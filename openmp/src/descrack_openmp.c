#include <omp.h>
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
     *   Svolgi in parallelo:
     *     Per ogni valore possibile tra 0 e pows[lunghezza attuale chiave]:
     *       Ottieni la chiave relativa al numero
     *       Testa la chiave, settando flag e valore in caso positivo
     * Se la chiave è stata trovata:
     *   Forza la parità della chiave
     *   Stampa la chiave
     */

    InputDESCrack input = inputDESCrackInit(argc, argv);
    DESBlock keyTemp = {0, 0}, key = {0, 0};
    bool keyFound = false;
    long pows[9] = {1};
    
    // Calcola tutte le potenze
    for (int i = 1; i < 9; i++)
        pows[i] = pows[i - 1] * input.alphabetLength;
    // Setta il numero di thread da utilizzare al valore massimo disponibile
    omp_set_num_threads(omp_get_max_threads());
    // Se la chiave non è stata ancora trovata, ripeti per ogni valore della lunghezza della chiave...
    for (int i = input.minKeyLength; i <= input.maxKeyLength && !keyFound; i++) {
        // Direttiva OpenMP per eseguire il for in parallelo sui vari thread
        //  mantenendo "privato" il valore di keyTemp
        // NOTA: OpenMP si occupa personalmente di dividere gli intervalli dei valori
        //  in parti "uguali"
        #pragma omp parallel for private(keyTemp)
        // Ripeti per ogni possibile valore...
        // NOTA: In OpenMP i for devono essere "canonici" (vedere le specifiche)
        for (long j = 0; j < pows[i]; j++) {
            // Se la chiave è stata trovata, passa rapidamente al valore successivo
            // NOTA: In OpenMP non è possibile utilizzare break nei loop
            if (keyFound)
                continue;
            // Converti il valore nella chiave specifica
            keyTemp = valueToKey(i, j, pows, input.alphabet);
            // Testa la chiave
            if (keyTest(&input.cipherTextBlock, &input.plainTextBlock, &keyTemp)) {
                keyFound = true;
                // keyTemp è "privato" al thread, quindi ho bisogno di una variabile
                //  condivisa per utilizzare la chiave in seguito
                key = keyTemp;
            }
        }
    }
    // Se la chiave è stata trovata...
    if (keyFound) {
        // ...forza la parità (dispari) e stampala
        forceOddParity(&key);
        printf("%08x%08x\n", key.hi, key.lo);
    }
    return EXIT_SUCCESS;
}