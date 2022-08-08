#include "RawSocket.h"

int main () {
    int teste = ConexaoRawSocket("lo");
    unsigned char batata;
    while (1) {
        recvfrom(teste, &batata, 1, 0, NULL, NULL);
        printf("%c\n", batata);
    }

    return 1;
}