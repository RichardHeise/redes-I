#include "RawSocket.h"

void unpack_msg(unsigned char* buf, int* type, int* old_seq) {

    msgHeader* header = (msgHeader *)(buf);
    int parity = 0;

    unsigned char* data = (sizeof(header) + buf);
    for (int i = 0; i < header->size; i++) {
        parity ^= data[i];
    }

    if (parity != data[header->size-1]) {
        *type = NACK;
        return;
    }

    int shouldBe_seq;
    if (*old_seq == 15) {
        shouldBe_seq = 0;
    } else {
        shouldBe_seq = *old_seq + 1;
    }

    if ( header->seq != shouldBe_seq ) {
        *type = NACK;
        return;
    }

    *old_seq = shouldBe_seq;
    *type = header->type;

}

void server_controller(int client) {

    unsigned char* buf = calloc(MAX_DATA_BYTES, sizeof(unsigned char));
    int seq_counter = -1;
    int type = DEFAULT;

    while (1) {
        if ( recvfrom(client, buf, MAX_DATA_BYTES, 0, NULL, 0) < 0) {
            perror("Error while receiving data. Aborting\n");
            exit(-2);
        }

        if (buf[0] == 126) {
            unpack_msg(buf, &type, &seq_counter);
        }
    }
}

int main () {
    int client = ConexaoRawSocket("lo");

    server_controller(client);

    return 1;
}