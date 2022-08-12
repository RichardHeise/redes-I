#include "RawSocket.h"
#include <sys/syscall.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h> 
#include <errno.h>

int counter_seq = 0;
int last_seq = 15;

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
                data[0]='G';    // erros que não foram definidos em sala
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

void server_controller(int client) {

    unsigned char* buf = calloc(MAX_DATA_BYTES, sizeof(unsigned char));

    while (1) {
        if ( recvfrom(client, buf, MAX_DATA_BYTES, 0, NULL, 0) < 0) {
            perror("Error while receiving data. Aborting\n");
            exit(-2);
        }

        if (buf[0] == INIT_MARKER) {
            
            if ( unpack_msg(buf, client, &counter_seq, &last_seq) )  
                choose_command(buf, client);
        }
    }
}

int main () {
    int client = ConexaoRawSocket("lo");

    server_controller(client);

    return 1;
}