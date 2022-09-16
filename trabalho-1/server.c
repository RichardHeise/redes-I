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

void reader(int socket,  unsigned char* buf, unsigned char* data, char* file) {
    FILE *reader = fopen(file,"rb");

    while(!feof(reader)) {
        memset(data, 0, DATA_BYTES);
        if ( fread(data, sizeof(unsigned char), DATA_BYTES-1, reader) ) {
            send_msg(socket, data, SENDING, &counter_seq);
        }

        while(1) {
            if ( recvfrom(socket, buf, MAX_DATA_BYTES, 0, NULL, 0) < 0) {
                perror("Error while receiving data. Aborting\n");
                exit(-2);
            }

            if (buf[0] == INIT_MARKER) {
                
                if ( unpack_msg(buf, socket, &counter_seq, &last_seq, 0) )  {
                    fprintf(stderr, "Recebi o pacote.\n");
                    int received = choose_command(buf, socket); 
                    if (received == ACK)

                        break;

                    else if (received == NACK) {

                        if (counter_seq == 0) {
                            counter_seq = 15;
                        } else {
                            counter_seq -= 1;
                        }

                        if (last_seq == 0) {
                            last_seq = 15;
                        } else {
                            last_seq -= 1;
                        }

                        send_msg(socket, data, SENDING, &counter_seq);
                    }
                }   
            }
        }
    }
    send_msg(socket, 0, END, &counter_seq);

    fclose(reader);
    system("rm -f file");
}

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

    unsigned char* data = calloc(DATA_BYTES, sizeof(unsigned char));

    reader(client, buf, data, "/tmp/saida.txt");
    
    free(data);
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
    int client = ConexaoRawSocket("enp7s0f0");
    //int client = ConexaoRawSocket("lo");

    system("clear");
    fprintf(stderr, "Servidor inicializado, aguardando pacotes.\n");
    server_controller(client);

    return 1;
}