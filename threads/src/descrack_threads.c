#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>

#include "des.h"
#include "helper_descrack.h"
#include "input_descrack.h"


/**
 * Struttura per passare argumenti multipli alla funzione chiamata da ogni thread.
 * 
 * @param lowerBound valore del limite inferiore dell'intervallo
 * @param upperBound valore del limite superiore dell'intervallo
 **/
typedef struct ThreadArgument {
    long lowerBound, upperBound;
} ThreadArgument;


// Variabili condivise tra i thread
// Non abbiamo un problema di concorrenza su key e keyFound, dato che
//  verranno scritte una volta sola da un singolo thread
static DESBlock cipherTextBlock = {0, 0};
static DESBlock plainTextBlock = {0, 0};
static DESBlock key = {0, 0};
static bool keyFound = false;
static char alphabet[128] = {0};
static long pows[9] = {1};
static int keyLength = 0;


/**
 * Calcola l'intervallo di iterazione per la generazione delle chiavi.
 * 
 * @param threads numero di thread
 * @param threadId id del thread
 * @param pows array contenente le potenze della dimensione dell'alfabeto
 * @param keyLength lunghezza attuale della chiave da cercare
 * @param lowerBound puntatore alla variabile che conterrà il limite inferiore dell'intervallo
 * @param upperBound puntatore alla variabile che conterrà il limite superiore dell'intervallo
 **/
static void setRange(const int threads, const int threadId, long *lowerBound, long *upperBound) {
    long part = pows[keyLength] / threads;

    // Tutti gli intervalli hanno dimensione identica, tranne l'ultimo che può essere più piccolo
    if (pows[keyLength] % threads)
        part++;
    *lowerBound = threadId * part;
    *upperBound = (threadId == threads - 1 && pows[keyLength] % threads) ? pows[keyLength] : *lowerBound + part;
}

/**
 * Funzione da eseguire al momento della creazione di un thread.
 * 
 * @param data puntatore all'argomento della funzione
 * 
 * @return il valore NULL
 **/
void *threadFunction(void *data) {
    ThreadArgument *argument = (ThreadArgument *) data;
    DESBlock keyTemp = {0, 0};

    // Se la chiave non è stata ancora trovata, ripeti per ogni possibile chiave...
    for (long i = argument->lowerBound; i < argument->upperBound && !keyFound; i++) {
        // Converti il valore nella chiave specifica
        keyTemp = valueToKey(keyLength, i, pows, alphabet);
        // Testa la chiave
        if(keyTest(&cipherTextBlock, &plainTextBlock, &keyTemp)) {
            keyFound = true;
            // keyTemp è locale al thread, quindi ho bisogno di una variabile
            //  condivisa per utilizzare la chiave in seguito
            key = keyTemp;
            break;
        }
    }
    // Libero memoria
    free(data);
    return NULL;
}

int main(int argc, char **argv) {
    /*
     * Algoritmo:
     * 
     * Imposta i valori delle variabili condivise
     * Calcola tutte le potenze da 0 a 8 (compreso) della dimensione dell'alfabeto
     * Per ogni valore della lunghezza della chiave e finchè la chiave non è stata trovata:
     *   Crea tanti thread quanti sono quelli disponibili
     *   Calcola l'intervallo di valori per ogni thread
     *   Esegui su ogni thread:
     *     Per ogni valore possibile nell'intervallo e finchè la chiave non è stata trovata:
     *       Ottieni la chiave relativa al valore
     *       Testa la chiave, settando flag e chiave in caso positivo
     * Se la chiave è stata trovata:
     *   Forza la parità della chiave
     *   Stampa la chiave
     */

    InputDESCrack input = inputDESCrackInit(argc, argv);
    // Numero di core totali
    int threadsNumber = get_nprocs_conf();
    pthread_t threads[threadsNumber];
    ThreadArgument *argument = NULL;

    memset(threads, 0, threadsNumber * sizeof(pthread_t));
    // Copia i valori nelle variabili condivise
    cipherTextBlock = input.cipherTextBlock;
    plainTextBlock = input.plainTextBlock;
    memcpy(alphabet, input.alphabet, input.alphabetLength);
    // Calcola tutte le potenze
    for (int i = 1; i < 9; i++)
        pows[i] = pows[i - 1] * input.alphabetLength;
    // Se la chiave non è stata ancora trovata, ripeti per ogni valore della lunghezza della chiave...
    for (keyLength = input.minKeyLength; keyLength <= input.maxKeyLength && !keyFound; keyLength++) {
        // Fork dei thread
        for (int j = 0; j < threadsNumber; j++) {
            // Alloca memoria per l'argomento da passare al thread
            argument = calloc(1, sizeof(ThreadArgument));
            // In caso di errore, stampa un messaggio ed esci
            if (argument == NULL) {
                fprintf(stderr, "Error during memory allocation\n");
                exit(EXIT_FAILURE);
            }
            // Calcola l'intervallo di iterazione relativo all'id del thread
            setRange(threadsNumber, j, &argument->lowerBound, &argument->upperBound);
            // Fork dei thread
            // In caso di errore, stampa un messaggio ed esci
            if (pthread_create(threads + j, NULL, threadFunction, argument) != 0) {
                fprintf(stderr, "Error during thread fork\n");
                exit(EXIT_FAILURE);
            }
        }
        for (int j = 0; j < threadsNumber; j++)
            // Join dei thread
            // In caso di errore, stampa un messaggio ed esci
            if(pthread_join(threads[j], NULL)) {
                fprintf(stderr, "Error during thread join\n");
                exit(EXIT_FAILURE);                
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