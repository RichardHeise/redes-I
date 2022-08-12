#include "RawSocket.h"
#include <sys/syscall.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h> 
#include <errno.h>

int counter_seq = -1;

void mkdir_server(unsigned char* buf, int client) {
    msgHeader* header = (msgHeader *)(buf);
    unsigned char* data = (sizeof(header) + buf);
    data[header->size] = '\0';

    if (!mkdir(data, 0700)) {

        send_msg(client, data, OK, &counter_seq);

    } else {

      switch (errno){
        case EACCES:
        case EFAULT:
            data[0]='B';    // retorna que não possui permissão de acesso
            break;
        case EEXIST:
            data[0]='C';    // diretório já existe
            break;
        case ENOSPC:
            data[0]='E';    // retorna que não há espaço em disco para criar diretório
            break; 
        default:
            data[0]='Z';    // erros que não foram definidos em sala
            break;
      }
      send_msg(client, data, ERROR, &counter_seq);

    }
}

void choose_command(unsigned char* buf, int client) {
    msgHeader* header = (msgHeader*)(buf);

    switch (header->type)
    {
    case MKDIR:
        mkdir_server(buf, client);
        break;
    
    default:
        break;
    }
}

int unpack_msg(unsigned char* buf, int client) {

    msgHeader* header = (msgHeader *)(buf);
    int parity = 0;

    unsigned char* data = (sizeof(header) + buf);
    for (int i = 0; i < header->size; i++) {
        parity ^= data[i];
    }
    
    if (parity != buf[MAX_DATA_BYTES-1]) {
        send_msg(client, NULL, NACK, &counter_seq);
        return 0;
    }

    int shouldBe_seq;
    if (counter_seq == 15) {
        shouldBe_seq = 0;
    } else {
        shouldBe_seq = counter_seq + 1;
    }

    if ( header->seq != shouldBe_seq ) {
        send_msg(client, NULL, NACK, &counter_seq);
        return 0;
    }

    return 1;

}

void server_controller(int client) {

    unsigned char* buf = calloc(MAX_DATA_BYTES, sizeof(unsigned char));

    while (1) {
        if ( recvfrom(client, buf, MAX_DATA_BYTES, 0, NULL, 0) < 0) {
            perror("Error while receiving data. Aborting\n");
            exit(-2);
        }

        if (buf[0] == 126) {
            if ( unpack_msg(buf, client) )  
                choose_command(buf, client);
        }
    }
}

int main () {
    int client = ConexaoRawSocket("lo");

    server_controller(client);

    return 1;
}