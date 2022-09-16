#include "RawSocket.h"
#include <sys/syscall.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h> 
#include <errno.h>

int counter_seq = 0;
int last_seq = 15;

int ls_response(unsigned char* buf, int socket) {
    msgHeader* header = (msgHeader*)(buf);

    print_msgHeader(header);

    switch (header->type) {
        case SENDING:
            fprintf(stderr, "%s", (buf + sizeof(msgHeader)) );
            break;
        case END:
            fprintf(stderr, "Mensagem recebida.\n");
            return 1;
        default:
            fprintf(stderr, "Tipo desconhecido.\n");
            break;
    }
    return 0;
}

void remote_ls(int socket, int opts) {
    unsigned char* data = calloc(DATA_BYTES, sizeof(unsigned char));
    data[0] = opts;
    send_msg(socket, data, RLS, &counter_seq);

    unsigned char* buf = calloc(MAX_DATA_BYTES, sizeof(unsigned char));

    while(1) {

        if ( recvfrom(socket, buf, MAX_DATA_BYTES, 0, NULL, 0) < 0) {
            perror("Error while receiving data. Aborting\n");
            exit(-2);
        }

        if (buf[0] == INIT_MARKER) {
            if ( unpack_msg(buf, socket, &counter_seq, &last_seq, RLS) )  {
                if (ls_response(buf, socket))
                    break;
            }
        }
        memset(buf,0,MAX_DATA_BYTES);
    }
    
    free(buf);
}

void print_error(unsigned char* buf) {
    char error = (buf+sizeof(msgHeader*))[0];

    switch(error) {
        case 'A':
            fprintf(stderr, "Diretório não existe.\n");
            break;
        case 'B':
            fprintf(stderr, "Sem permissão de acesso.\n");
            break;
        case 'C':
            fprintf(stderr, "Diretório já existe.\n");
            break;
        case 'E':
            fprintf(stderr, "Falta espaço em disco.\n");
            break;
        default:
            fprintf(stderr, "Erro desconhecido.\n");
            break;
    }
}

int choose_response(unsigned char* buf, int server) {
    msgHeader* header = (msgHeader*)(buf);

    switch (header->type) {
        case NACK:
            fprintf(stderr, "ue");
            send_msg(server, 0, NACK, &counter_seq);
            return 1;
        case ERROR:
            print_error(buf);
            return 1;
        case OK:
            fprintf(stderr, "Servidor executou a ação com sucesso.\n");
            return 1;
        default:
            fprintf(stderr, "Tipo desconhecido.\n");
            break;
    }
    return 0;
}

void remote_cmd(int server, unsigned char* dir, int type) {
    send_msg(server, dir, type, &counter_seq);

    unsigned char* buf = calloc(MAX_DATA_BYTES, sizeof(unsigned char));

    while(1) {

        if ( recvfrom(server, buf, MAX_DATA_BYTES, 0, NULL, 0) < 0) {
            perror("Error while receiving data. Aborting\n");
            exit(-2);
        }

        if (buf[0] == INIT_MARKER) {
            if ( unpack_msg(buf, server, &counter_seq, &last_seq, type) )  {
                choose_response(buf, server);
                break;
            }
        }
        memset(buf,0,MAX_DATA_BYTES);
    }
    
    free(buf);
}

void local_mkdir(unsigned char* dir) {
    mkdir(dir, 0700);
}

void local_cd(unsigned char* dir) {
    chdir(dir);
}

void local_ls(int opts) {
    if (opts == 0) {
        system("ls");
    } else if (opts == 1) {
        system("ls -a");
    } else if (opts == 2) {
        system("ls -l");
    } else {
        system("ls -a -l");
    }
}

int get_type(unsigned char* buf, int *opts, unsigned char* dir) {

    int opt_a = 0;
    int opt_l = 0;
    unsigned char command[7];
    unsigned char opt1[63];
    unsigned char opt2[6];

    sscanf(buf,"%s %s %s", command, opt1, opt2);
    if(!strncmp(command,"ls", 3)){

        if(!strncmp("-a", opt1, 3) || !strncmp("-a", opt2, 3))
            opt_a = 1;
        if(!strncmp("-l", opt1, 3) || !strncmp("-l", opt2, 3))
            opt_l = 1;

        if(opt_a == 1 && opt_l == 0)
            *opts = 1;
        if(opt_a ==0 && opt_l == 1)
            *opts = 2;
        if(opt_a == 1 && opt_l == 1)
            *opts = 3;

        return LS;

    }

    if(!strncmp("rls",command,3)) {

        if(!strncmp("-a", opt1, 3) || !strncmp("-a", opt2, 3))
            opt_a = 1;
        if(!strncmp("-l", opt1, 3) || !strncmp("-l", opt2, 3))
            opt_l = 1;
        if(opt_a == 1 && opt_l == 0)
            *opts = 1;
        if(opt_a == 0 && opt_l == 1)
            *opts = 2;
        if(opt_a == 1 && opt_l == 1)
            *opts = 3;

        return RLS;
    }
    if(!strncmp("cd", command, 3)){
        strncpy(dir, opt1, 63);
        return CD;
    }
    if(!strncmp("rcd", command, 3)){
        strncpy(dir, opt1, 63);
        return RCD;
    }
    if(!strncmp("mkdir", command, 6)){
        strncpy(dir, opt1, 63);
        return MKDIR;
    }
    if(!strncmp("rmkdir", command, 6)){
        strncpy(dir, opt1, 63);
        return RMKDIR;
    }
    else
        return 0;
}

void client_controller(int server) {
    unsigned char* input = calloc(MAX_DATA_BYTES, sizeof(unsigned char));
    unsigned char* dir = calloc(MAX_DATA_BYTES, sizeof(unsigned char));
    unsigned char* data = calloc(DATA_BYTES, sizeof(unsigned char));

    int type, ls_opts;

    while(1) {

        char rel_path[MAX_DATA_BYTES];
        fprintf(stderr,"%s:\n-> ", getcwd(rel_path, sizeof(rel_path)));
        
        fgets(input, MAX_DATA_BYTES, stdin);

        ls_opts = 0;
        type = get_type(input, &ls_opts, dir);

        switch(type) {
            case RLS:
                remote_ls(server, ls_opts);
                break;
            case LS:
                local_ls(ls_opts);
                break;
            case RCD:
                remote_cmd(server, dir, RCD);
                break;
            case CD:
                local_cd(dir);
                break;
            case RMKDIR:
                remote_cmd(server, dir, RMKDIR);
                break;
            case MKDIR:
                local_mkdir(dir);
                break;
            default:
                break;
        }

        memset(input, 0, MAX_DATA_BYTES);
        memset(data, 0, DATA_BYTES);
    }
}

int main () {

    int server = ConexaoRawSocket("enp7s0f0");
    // int server = ConexaoRawSocket("lo");

    if (server < 0) {
        perror("Error while creating socket, aborting.\n");
        exit(-1);
    }
    
    system("clear");
    client_controller(server);


    return 1;
}