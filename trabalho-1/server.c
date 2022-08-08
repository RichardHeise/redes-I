#include "RawSocket.h"

int main () {
    int client = ConexaoRawSocket("lo");
    msg_t *message;

    message->init_mark = INIT_MARKER;
    message->size = 5;
    message->seq = 0;
    message->type = 0;
    message->parity = 0;

    while (1) {
        sendto(client, message, 32, 0, NULL, NULL);
    }

    return 1;
}