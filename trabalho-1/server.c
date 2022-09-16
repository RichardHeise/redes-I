#include "RawSocket.h"
#include <sys/syscall.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h> 
#include <errno.h>
#include <math.h>

int counter_seq = 0;
int last_seq = 15;

int choose_command(unsigned char* buf, int client);

void ls_server(unsigned char* buf, int client) {

    msgHeader* header = (msgHeader *)(buf);
    int opts = (buf + sizeof(msgHeader))[0];

    if (opts == 0) {
        system("ls > /tmp/saida.txt");
    } else if (opts == 1) {
        system("ls -a > /tmp/saida.txt");
    } else if (opts == 2) {
        system("ls -l > /tmp/saida.txt");
    } else {
        system("ls -a -l > /tmp/saida.txt");
    }

    reader(client, "/tmp/saida.txt", &counter_seq, &last_seq);
    
}

void cd_server(unsigned char* buf, int client) {

    msgHeader* header = (msgHeader *)(buf);

    unsigned char* data = (sizeof(msgHeader) + buf);
    data[header->size] = '\0';

    if (!chdir(data)) {
        
        send_msg(client, data, OK, &counter_seq);

        fprintf(stderr, "cd remoto funcionou.\n");

    } else {

        fprintf(stderr, "cd remoto falhou, mandando erro para o cliente.\n");
        switch (errno){
            case EACCES:
            case EFAULT:
                data[0]='B';    // retorna que não possui permissão de acesso
                break;
            case ENOTDIR:
            case ENOENT:
                data[0]='A';    // retorna que o diretório não exite
                break;
            default:
                data[0]='Z';    // erros que não foram definidos em sala
                break;
        }
       
        send_msg(client, data, ERROR, &counter_seq);
    }
}

void mkdir_server(unsigned char* buf, int client) {
    
    msgHeader* header = (msgHeader *)(buf);
    unsigned char* data = (sizeof(msgHeader) + buf);
    data[header->size] = '\0';

    if (!mkdir(data, 0700)) {
        
        send_msg(client, data, OK, &counter_seq);

        fprintf(stderr, "mkdir remoto funcionou.\n");

    } else {

        fprintf(stderr, "mkdir remoto falhou, mandando erro para o cliente.\n");
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

void put_server(unsigned char* buf, int socket) {
    msgHeader* header = (msgHeader *)(buf);
    unsigned char* name = (sizeof(msgHeader) + buf);
    FILE* writer = fopen(name, "w");
    
    while(1) {
        if ( recvfrom(socket, buf, MAX_DATA_BYTES, 0, NULL, 0) < 0) {
            perror("Error while receiving data. Aborting\n");
            exit(-2);
        }

        if (buf[0] == INIT_MARKER) {
            if ( unpack_msg(buf, socket, &counter_seq, &last_seq, ACK) )  {
                if (header->type == SENDING) {
                    print_msgHeader(header);
                    fprintf(writer, "%s", buf+sizeof(msgHeader));
                    send_msg(socket, 0, ACK, &counter_seq);
                } else if (header->type == END) {
                    send_msg(socket, 0, ACK, &counter_seq);
                    break;
                }
            }
        }
        send_msg(socket, 0, ACK, &counter_seq);
        memset(buf,0,MAX_DATA_BYTES);
    }

    fclose(writer);
}

int choose_command(unsigned char* buf, int client) {
    msgHeader* header = (msgHeader*)(buf);
    
    switch (header->type) {
        case RMKDIR:
            fprintf(stderr, "Executando um mkdir.\n");
            mkdir_server(buf, client);
            break;
        case RCD:
            fprintf(stderr, "Executando um cd remoto.\n");
            cd_server(buf, client);
            break;
        case RLS:
            fprintf(stderr, "Executando um ls remoto.\n");
            ls_server(buf, client);
            break;
        case PUT:
            send_msg(client, 0, ACK, &counter_seq);
            fprintf(stderr, "Recebendo dados.");
            put_server(buf, client);
            break;
        case GET:
            put(client, buf+sizeof(msgHeader), &counter_seq, &last_seq);
        case ACK:
            return ACK;
        case NACK:
            return NACK;
        default:
            break;
    }

    return 0;
}

void server_controller(int client) {

    unsigned char* buf = calloc(MAX_DATA_BYTES, sizeof(unsigned char));

    while (1) {
        if ( recvfrom(client, buf, MAX_DATA_BYTES, 0, NULL, 0) < 0) {
            perror("Error while receiving data. Aborting\n");
            exit(-2);
        }

        if (buf[0] == INIT_MARKER) {
            
            if ( unpack_msg(buf, client, &counter_seq, &last_seq, 0) )  {
                fprintf(stderr, "Recebi o pacote.\n");
                choose_command(buf, client);
            }
        }

        memset(buf,0,MAX_DATA_BYTES);
    }

    free(buf);
}

int main () {
    int client = ConexaoRawSocket("eno1");
    //int client = ConexaoRawSocket("lo");

    system("clear");
    fprintf(stderr, "Servidor inicializado, aguardando pacotes.\n");
    server_controller(client);

    return 1;
}