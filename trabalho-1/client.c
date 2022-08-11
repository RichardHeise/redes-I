#include "RawSocket.h"
#include <sys/syscall.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h> 

void printf_msgHeader(msgHeader* header) {
    printf(
        "Init marker: %d\nseq: %d\nsize: %d\ntype: %d\n",
        header->init_mark, header->seq, header->size, header->type
    );
}

void create_msgHeader(msgHeader* header, int seq, int size, int type) {

    header->init_mark = INIT_MARKER;
    header->seq = seq;
    header->size = size;
    header->type = type;

}

void send_msg(int server, unsigned char* data, int type, int seq) {
    unsigned char* buf = calloc(MAX_DATA_BYTES, sizeof(unsigned char));
    msgHeader* header = (msgHeader *)(buf);

    create_msgHeader(header, seq, strlen(data), type);

    int msg_size = sizeof(header)+header->size;
    int parity = 0;

    for (int i = sizeof(header); i < msg_size; i++) {
        buf[i] = data[i - sizeof(header)];
        parity ^= buf[i];
    }

    buf[MAX_DATA_BYTES-1] = parity;
    sendto(server, buf, MAX_DATA_BYTES, 0, NULL, 0);
    free(buf);
}

void remote_mkdir(int server, unsigned char* dir, int type, int* seq) {
    if (*seq == 15) 
        *seq = 0;
    else 
        *seq += 1;
    
    send_msg(server, dir, type, *seq);
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

        return LSL;

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

        return LS;
    }
    if(!strncmp("cd", command, 3)){
        strncpy(dir, opt1, 63);
        return CDL;
    }
    if(!strncmp("rcd", command, 3)){
        strncpy(dir, opt1, 63);
        return CD;
    }
    if(!strncmp("mkdir", command, 6)){
        strncpy(dir, opt1, 63);
        return MKDIRL;
    }
    if(!strncmp("rmkdir", command, 6)){
        strncpy(dir, opt1, 63);
        return MKDIR;
    }
    else
        return 0;
}

void client_controller(int server) {
    unsigned char* input = calloc(MAX_DATA_BYTES, sizeof(unsigned char));
    unsigned char* dir = calloc(MAX_DATA_BYTES, sizeof(unsigned char));
    unsigned char* data = calloc(DATA_BYTES, sizeof(unsigned char));

    int type, ls_opts;
    int seq_counter = -1;

    while(1) {
        char rel_path[MAX_DATA_BYTES];
        fprintf(stderr,"%s:\n-> ", getcwd(rel_path, sizeof(rel_path)));
        
        fgets(input, MAX_DATA_BYTES, stdin);

        ls_opts = 0;
        type = get_type(input, &ls_opts, dir);

        switch(type) {
            case LS:
                //remote_ls();
                break;
            case LSL:
                local_ls(ls_opts);
                break;
            case CD:
                //remote_cd();
                break;
            case CDL:
                local_cd(dir);
                break;
            case MKDIR:
                remote_mkdir(server, dir, type, &seq_counter);
                break;
            case MKDIRL:
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

    int server = ConexaoRawSocket("lo");

    if (server < 0) {
        perror("Error while creating socket, aborting.\n");
        exit(-1);
    }

    system("clear");
    client_controller(server);


    return 1;
}