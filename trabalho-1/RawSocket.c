#include "RawSocket.h"

int ConexaoRawSocket(char *device) {

    int soquete;
    struct ifreq ir;
    struct sockaddr_ll endereco;
    struct packet_mreq mr;

    soquete = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));  	/*cria socket*/
    if (soquete == -1) {
        printf("Erro no Socket\n");
        exit(-1);
    }

    memset(&ir, 0, sizeof(struct ifreq));  	/*dispositivo eth0*/
    memcpy(ir.ifr_name, device, sizeof(device));
    if (ioctl(soquete, SIOCGIFINDEX, &ir) == -1) {
        printf("Erro no ioctl\n");
        exit(-1);
    }
        

    memset(&endereco, 0, sizeof(endereco)); 	/*IP do dispositivo*/
    endereco.sll_family = AF_PACKET;
    endereco.sll_protocol = htons(ETH_P_ALL);
    endereco.sll_ifindex = ir.ifr_ifindex;
    if (bind(soquete, (struct sockaddr *)&endereco, sizeof(endereco)) == -1) {
        printf("Erro no bind\n");
        exit(-1);
    }


    memset(&mr, 0, sizeof(mr));          /*Modo Promiscuo*/
    mr.mr_ifindex = ir.ifr_ifindex;
    mr.mr_type = PACKET_MR_PROMISC;
    if (setsockopt(soquete, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) == -1)	{
        printf("Erro ao fazer setsockopt\n");
        exit(-1);
    }

    return soquete;
}

void create_msgHeader(msgHeader* header, int seq, int size, int type) {

    header->init_mark = INIT_MARKER;
    header->seq = seq;
    header->size = size;
    header->type = type;

}

void print_msgHeader(msgHeader* header) {
    printf(
        "Init marker: %d\nseq: %d\nsize: %d\ntype: %d\n",
        header->init_mark, header->seq, header->size, header->type
    );
}

void send_msg(int socket, unsigned char* data, int type, int* seq) {
    
    unsigned char* buf = calloc(MAX_DATA_BYTES, sizeof(unsigned char));
    msgHeader* header = (msgHeader *)(buf);

    if (data) {
        create_msgHeader(header, *seq, strlen(data), type);
    } else {
        create_msgHeader(header, *seq, 0, type);
    }

    int msg_size = sizeof(header)+header->size;
    int parity = 0;

    for (int i = sizeof(header); i < msg_size; i++) {
        buf[i] = data[i - sizeof(header)];
        parity ^= buf[i];
    }

    buf[MAX_DATA_BYTES-1] = parity;

    sendto(socket, buf, MAX_DATA_BYTES, 0, NULL, 0);
    inc_seq(seq);

    free(buf);
}

void inc_seq(int* counter) {
    *counter = ((*counter+1) % 16);
}

int unpack_msg(unsigned char* buf, int socket, int* seq, int* last_seq) {

    msgHeader* header = (msgHeader *)(buf);

    if (*last_seq == header->seq) {
        return 0;
    }

    int parity = 0;
    
    unsigned char* data = (sizeof(header) + buf);
    for (int i = 0; i < header->size; i++) {
        parity ^= data[i];
    }
    
    if (parity != buf[MAX_DATA_BYTES-1]) {
        send_msg(socket, NULL, NACK, seq);
        return 0;
    }

    int shouldBe_seq = *last_seq;

    inc_seq(&shouldBe_seq);


    if ( header->seq != shouldBe_seq ) {
        send_msg(socket, NULL, NACK, seq);
        return 0;
    }

    *last_seq = header->seq;
    return 1;

}