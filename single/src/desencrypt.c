#include <stdio.h>
#include <stdlib.h>

#include "des.h"
#include "input_desencrypt.h"


int main(int argc, char **argv) {
    InputDESEncrypt input = inputDESEncryptInit(argc, argv);
    DESBlock cipherTextBlock = {0, 0};

    // Forza la parità (dispari)
    forceOddParity(&input.key);
    // Cifra il testo in chiaro e stampa il blocco cifrato
    cipherTextBlock = encrypt(&input.plainTextBlock, &input.key);
    printf("%08x%08x\n", cipherTextBlock.hi, cipherTextBlock.lo);
    return EXIT_SUCCESS;
}