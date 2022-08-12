#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <linux/if.h>
#include <stdlib.h>
#include <arpa/inet.h> 
#include <string.h>
#include <stdio.h>

enum fields {
    INIT_MARKER = 126,
    OK = 1,
    NACK = 2,
    ACK = 3,
    CD = 6,
    LS = 7,
    MKDIR = 8,
    GET = 9,
    PUT = 10,
    ERROR = 17,
    DESCRIPTOR = 24,
    DATA_BYTES = 63,
    LSL = 35,
    CDL = 36,
    MKDIRL = 37,
    END = 46,
    MAX_DATA_BYTES = 67,
    DEFAULT = 666
};

typedef struct __attribute__((packed)) msg_s {
    unsigned int init_mark : 8;
    unsigned int size : 6;
    unsigned int seq : 4;
    unsigned int type : 6;
} msgHeader;

int ConexaoRawSocket(char *device);

void create_msgHeader(msgHeader* header, int seq, int size, int type);

void printf_msgHeader(msgHeader* header);

void send_msg(int socket, unsigned char* data, int type, int* seq);

void inc_seq(int* counter);