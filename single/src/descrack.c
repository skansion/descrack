#include <stdio.h>
#include <stdlib.h>

#include "des.h"
#include "helper_descrack.h"
#include "input_descrack.h"


int main(int argc, char **argv) {
    /*
     * Algoritmo:
     * 
     * Calcola tutte le potenze da 0 a 8 (compreso) della dimensione dell'alfabeto
     * Per ogni valore della lunghezza della chiave e finchè la chiave non è stata trovata:
     *   Per ogni valore possibile tra 0 e pows[lunghezza attuale chiave] e finchè la chiave non è stata trovata:
     *     Ottieni la chiave relativa al valore
     *     Testa la chiave, settando il flag in caso positivo
     * Se la chiave è stata trovata:
     *   Forza la parità della chiave
     *   Stampa la chiave
     */

    InputDESCrack input = inputDESCrackInit(argc, argv);
    DESBlock keyTemp = {0, 0};
    bool keyFound = false;
    long pows[9] = {1};

    // Calcola tutte le potenze
    for (int i = 1; i < 9; i++)
        pows[i] = pows[i - 1] * input.alphabetLength;
    // Se la chiave non è stata ancora trovata, ripeti per ogni valore della lunghezza della chiave...
    for (int i = input.minKeyLength; i <= input.maxKeyLength && !keyFound; i++)
        // Se la chiave non è stata ancora trovata, ripeti per ogni possibile chiave...
        for (long j = 0; j < pows[i] && !keyFound; j++) {
            // Converti il valore nella chiave specifica
            keyTemp = valueToKey(i, j, pows, input.alphabet);
            // Testa la chiave
            keyFound = keyTest(&input.cipherTextBlock, &input.plainTextBlock, &keyTemp);
        }
    // Se la chiave è stata trovata...
    if (keyFound) {
        // ...forza la parità (dispari) e stampala
        forceOddParity(&keyTemp);
        printf("%08x%08x\n", keyTemp.hi, keyTemp.lo);
    }
    return EXIT_SUCCESS;
}