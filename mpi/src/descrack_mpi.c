#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include "des.h"
#include "helper_descrack.h"
#include "input_descrack.h"


/**
 * Calcola l'intervallo di iterazione per la generazione delle chiavi.
 * 
 * @param size dimensione del communicator
 * @param rank rank del processo
 * @param pows array contenente le potenze della dimensione dell'alfabeto
 * @param keyLength lunghezza attuale della chiave da cercare
 * @param lowerBound puntatore alla variabile che conterrà il limite inferiore dell'intervallo
 * @param upperBound puntatore alla variabile che conterrà il limite superiore dell'intervallo
 **/
static void setRangeMPI(const int size, const int rank, const long *pows, const int keyLength, long *lowerBound, long *upperBound) {
    long part = pows[keyLength] / size;

    // Tutti gli intervalli hanno dimensione identica, tranne l'ultimo che può essere più piccolo
    if (pows[keyLength] % size)
        part++;
    *lowerBound = rank * part;
    *upperBound = (rank == size - 1 && pows[keyLength] % size) ? pows[keyLength] : *lowerBound + part;
}

int main(int argc, char **argv) {
    /*
     * Algoritmo:
     * 
     * Calcola tutte le potenze da 0 a 8 (compreso) della dimensione dell'alfabeto
     * Se il processo ha rank diverso da "size - 1":
     *   Preparati alla ricezione di un bcast asincrono dal processo con rank "size - 1":
     *     Per ogni valore della lunghezza della chiave e finchè la chiave non è stata trovata:
     *       Imposta i valori dell'intervallo su cui iterare in base a size, rank, valori possibili e dimensione alfabeto
     *       Per ogni valore nell'intervallo e finchè la chiave non è stata trovata:
     *         Controlla se il bcast è arrivato
     *         Se il bcast non è ancora arrivato:
     *           Ottieni la chiave relativa al valore
     *           Testa la chiave
     *           Se la chiave è stata trovata:
     *             Manda un messaggio asincrono al processo con rank "size - 1" e aspetta la ricezione
     *         Altrimenti:
     *           Setta il flag "chiave trovata"
     *       Attendi la fine degli altri processi
     * Altrimenti:
     *   Preparati alla ricezione di un messaggio asincrono da un qualsiasi processo
     *     Per ogni valore della lunghezza della chiave e finchè la chiave non è stata trovata:
     *       Imposta i valori dell'intervallo su cui iterare in base a size, rank, valori possibili e dimensione alfabeto
     *       Per ogni valore nell'intervallo e finchè la chiave non è stata trovata:
     *         Controlla se il messaggio è arrivato
     *         Se il messaggio non è ancora arrivato:
     *           Ottieni la chiave relativa al valore
     *           Testa la chiave
     *           Se la chiave è stata trovata:
     *             Manda un bcast asincrono e aspetta la ricezione
     *         Altrimenti:
     *           Setta il flag "chiave trovata"
     *           Manda un bcast asincrono e aspetta la ricezione
     *       Attendi la fine degli altri processi
     * Attendi la fine degli altri processi
     * Se il processo è quello che ha trovato la chiave:
     *   Forza la parità della chiave
     *   Stampa la chiave
     */

    InputDESCrack input = inputDESCrackInit(argc, argv);
    DESBlock keyTemp = {0, 0};
    int keyFoundFlag = 0, isReceived = 0, size = 0, rank = 0;
    long pows[9] = {1}, lowerBound = 0, upperBound = 0;
    MPI_Request request;

    MPI_Init(NULL, NULL);
    // Imposto size e rank del processo
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // Calcolo tutte le potenze
    for (int i = 1; i < 9; i++)
        pows[i] = pows[i - 1] * input.alphabetLength;
    // Se il processo non è "l'ultimo"...
    if (rank != size - 1) {
        // Prepara la ricezione di un bcast asincrono dal processo con rank = size - 1
        MPI_Ibcast(NULL, 0, MPI_INT, size - 1, MPI_COMM_WORLD, &request);
        // Se la chiave non è stata ancora trovata, ripeti per ogni valore della lunghezza della chiave...
        for (int i = input.minKeyLength; i <= input.maxKeyLength && !keyFoundFlag; i++) {
            // Calcola l'intervallo di iterazione relativo al rank del processo
            setRangeMPI(size, rank, pows, i, &lowerBound, &upperBound);
            // Se la chiave non è stata ancora trovata, ripeti per ogni possibile valore...
            for (long j = lowerBound; j < upperBound && !keyFoundFlag; j++) {
                // Testa la ricezione del bcast
                MPI_Test(&request, &isReceived, MPI_STATUS_IGNORE);
                // Se il bcast non è arrivato...
                if (!isReceived) {
                    // Converti il valore nella chiave specifica
                    keyTemp = valueToKey(i, j, pows, input.alphabet);
                    // Testa la chiave
                    keyFoundFlag = keyTest(&input.cipherTextBlock, &input.plainTextBlock, &keyTemp);
                    if (keyFoundFlag) {
                        // Manda un messaggio asincrono al processo con rank = size - 1
                        MPI_Isend(NULL, 0, MPI_INT, size - 1, 0, MPI_COMM_WORLD, &request);
                        // Aspetta la ricezione
                        MPI_Wait(&request, MPI_STATUS_IGNORE);
                    }
                }
                else
                    keyFoundFlag = 1;
            }
            // Attendi il termine dell'iterazione degli altri processi
            MPI_Barrier(MPI_COMM_WORLD);
        }
    }
    // ...altrimenti
    else {
        // Prepara la ricezione di un messaggio asincrono da un qualsiasi processo
        MPI_Irecv(NULL, 0, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &request);
        // Se la chiave non è stata ancora trovata, ripeti per ogni valore della lunghezza della chiave...
        for (int i = input.minKeyLength; i <= input.maxKeyLength && !keyFoundFlag; i++) {
            // Calcola l'intervallo di iterazione relativo al rank del processo
            setRangeMPI(size, rank, pows, i, &lowerBound, &upperBound);
            // Se la chiave non è stata ancora trovata, ripeti per ogni possibile valore...
            for (long j = lowerBound; j < upperBound && !keyFoundFlag; j++) {
                // Testa la ricezione del messaggio
                MPI_Test(&request, &isReceived, MPI_STATUS_IGNORE);
                // Se il messaggio non è arrivato...
                if (!isReceived) {
                    // Converti il valore nella chiave specifica
                    keyTemp = valueToKey(i, j, pows, input.alphabet);
                    // Testa la chiave
                    keyFoundFlag = keyTest(&input.cipherTextBlock, &input.plainTextBlock, &keyTemp);
                    if (keyFoundFlag) {
                        // Manda un bcast asincrono e aspetta la ricezione
                        MPI_Ibcast(NULL, 0, MPI_INT, rank, MPI_COMM_WORLD, &request);
                        MPI_Wait(&request, MPI_STATUS_IGNORE);
                    }
                }
                else {
                    keyFoundFlag = 1;
                    // Manda un bcast asincrono e aspetta la ricezione
                    MPI_Ibcast(NULL, 0, MPI_INT, rank, MPI_COMM_WORLD, &request);
                    MPI_Wait(&request, MPI_STATUS_IGNORE);
                }
            }
            // Attendi il termine dell'iterazione degli altri processi
            MPI_Barrier(MPI_COMM_WORLD);
        }
    }
    // Attendi il termine dell'iterazione degli altri processi
    MPI_Barrier(MPI_COMM_WORLD);
    // Se la chiave è stata trovata in questo processo...
    if (keyFoundFlag && !isReceived) {
        // ...forza la parità (dispari) e stampala
        forceOddParity(&keyTemp);
        printf("%08x%08x\n", keyTemp.hi, keyTemp.lo);
    }
    MPI_Finalize();
    return EXIT_SUCCESS;
}