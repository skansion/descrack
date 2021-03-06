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
     *   Per ogni valore della lunghezza della chiave e finchè la chiave non è stata trovata:
     *     Imposta i valori dell'intervallo su cui iterare in base a size, rank, valori possibili e dimensione alfabeto
     *     Per ogni valore nell'intervallo e finchè la chiave non è stata trovata:
     *       Ottieni la chiave relativa al valore
     *       Testa la chiave
     *     Se la chiave è stata trovata in questo processo:
     *       Invia un messaggio asincrono al processo con rank "size - 1" e aspetta la ricezione
     *     Attendi l'invio del bcast da parte del processo con rank "size -1"
     *     Leggi il bcast e setta il rispettivo flag
     *   Attendi la fine degli altri processi
     * Altrimenti:
     *   Preparati alla ricezione di un messaggio asincrono da un qualsiasi processo
     *     Per ogni valore della lunghezza della chiave e finchè la chiave non è stata trovata:
     *       Imposta i valori dell'intervallo su cui iterare in base a size, rank, valori possibili e dimensione alfabeto
     *       Per ogni valore nell'intervallo e finchè la chiave non è stata trovata:
     *         Ottieni la chiave relativa al valore
     *         Testa la chiave
     *         Se la chiave è stata trovata in questo processo:
     *             Invia un bcast
     *             Attendi che gli altri processi siano pronti a ricevere
     *         Altrimenti:
     *           Controlla se sia arrivato un messaggio asincrono
     *           Se il messaggio è arrivato:
     *             Invia un bcast
     *             Attendi che gli altri processi siano pronti a ricevere
     *       Attendi la fine degli altri processi
     * Attendi la fine degli altri processi
     * Se il processo è quello che ha trovato la chiave:
     *   Forza la parità della chiave
     *   Stampa la chiave
     */

    InputDESCrack input = inputDESCrackInit(argc, argv);
    DESBlock keyTemp = {0, 0};
    int size = 0, rank = 0, received = 0;
    bool keyFound = false, keyFoundOutside = false;
    long pows[9] = {1}, lowerBound = 0, upperBound = 0;
    MPI_Request request;

    // Inizio sezione MPI
    MPI_Init(NULL, NULL);
    // Imposto size e rank del processo
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // Calcola tutte le potenze
    for (int i = 1; i < 9; i++)
        pows[i] = pows[i - 1] * input.alphabetLength;
    // Se il processo non è "l'ultimo"...
    if (rank != size - 1) {
        // Se la chiave non è stata ancora trovata, ripeti per ogni valore della lunghezza della chiave...
        for (int i = input.minKeyLength; i <= input.maxKeyLength && !keyFound && !keyFoundOutside; i++) {
            // Calcola l'intervallo di iterazione relativo al rank del processo
            setRangeMPI(size, rank, pows, i, &lowerBound, &upperBound);
            // Se la chiave non è stata ancora trovata, ripeti per ogni possibile valore...
            for (long j = lowerBound; j < upperBound && !keyFound && !keyFoundOutside; j++) {
                // Converti il valore nella chiave specifica
                keyTemp = valueToKey(i, j, pows, input.alphabet);
                // Testa la chiave
                keyFound = keyTest(&input.cipherTextBlock, &input.plainTextBlock, &keyTemp);
            }
            // Se la chiave è stata trovata...
            if (keyFound) {
                // Manda un messaggio asincrono al processo con rank "size - 1" e aspetta la ricezione
                MPI_Isend(NULL, 0, MPI_INT, size - 1, 0, MPI_COMM_WORLD, &request);
                MPI_Wait(&request, MPI_STATUS_IGNORE);
            }
            // Attendi che il bcast sia inviato
            MPI_Barrier(MPI_COMM_WORLD);
            // Ricevi il bcast
            MPI_Bcast(&keyFoundOutside, 1, MPI_C_BOOL, size - 1, MPI_COMM_WORLD);
            // Attendi il termine dell'iterazione degli altri processi
            MPI_Barrier(MPI_COMM_WORLD);
        }
    }
    // ...altrimenti
    else {
        // Prepara la ricezione di un messaggio asincrono da un qualsiasi processo
        MPI_Irecv(NULL, 0, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &request);
        // Se la chiave non è stata ancora trovata, ripeti per ogni valore della lunghezza della chiave...
        for (int i = input.minKeyLength; i <= input.maxKeyLength && !keyFound && !keyFoundOutside; i++) {
            // Calcola l'intervallo di iterazione relativo al rank del processo
            setRangeMPI(size, rank, pows, i, &lowerBound, &upperBound);
            // Se la chiave non è stata ancora trovata, ripeti per ogni possibile valore...
            for (long j = lowerBound; j < upperBound && !keyFound && !keyFoundOutside; j++) {
                // Converti il valore nella chiave specifica
                keyTemp = valueToKey(i, j, pows, input.alphabet);
                // Testa la chiave
                keyFound = keyTest(&input.cipherTextBlock, &input.plainTextBlock, &keyTemp);    
            }
            // Se la chiave è stata trovata...
            if (keyFound) {
                // Invia un bcast agli altri processi
                MPI_Bcast(&keyFound, 1, MPI_C_BOOL, rank, MPI_COMM_WORLD);
                // Attendi che gli altri processi siano pronti a leggere
                MPI_Barrier(MPI_COMM_WORLD);
            }
            // ...altrimenti
            else {
                // Testa se il messaggio asincrono sia arrivato
                MPI_Test(&request, &received, MPI_STATUS_IGNORE);
                // Se il messaggio è arrivato...
                keyFoundOutside = received == 1;
                // Invia un bcast agli altri processi
                MPI_Bcast(&keyFoundOutside, 1, MPI_C_BOOL, rank, MPI_COMM_WORLD);
                // Attendi che gli altri processi siano pronti a leggere
                MPI_Barrier(MPI_COMM_WORLD);
            }
            // Attendi il termine dell'iterazione degli altri processi
            MPI_Barrier(MPI_COMM_WORLD);
        }
    }
    // Attendi il termine dell'iterazione degli altri processi
    MPI_Barrier(MPI_COMM_WORLD);
    // Fine sezione MPI
    MPI_Finalize();
    // Se la chiave è stata trovata in questo processo...
    if (keyFound) {
        // ...forza la parità (dispari) e stampala
        forceOddParity(&keyTemp);
        printf("%08x%08x\n", keyTemp.hi, keyTemp.lo);
    }
    return EXIT_SUCCESS;
}
