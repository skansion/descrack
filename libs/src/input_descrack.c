#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "input_common.h"
#include "input_descrack.h"
#include "input_messages.h"


// Valori ASCII delle cifre
static const char DIGITS[10] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57};
// Valori ASCII delle lettere minuscole
static const char LOWER[26] = { 97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
                               110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122};
// Valori ASCII delle lettere maiuscole
static const char UPPER[26] = {65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77,
                               78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90};
// Valori ASCII dei simboli
static const char SYMBOLS[33] = { 32, 33, 34, 35, 36, 37, 38,  39,  40,  41,  42,
                                  43, 44, 45, 46, 47, 58, 59,  60,  61,  62,  63,
                                  64, 91, 92, 93, 94, 95, 96, 123, 124, 125, 126};
// Valori ASCII dei caratteri di controllo
static const char CONTROL[33] = { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,
                                 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,  21,
                                 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 127};


/**
 * Converte un argormento in numero.
 * 
 * @param argument argomento da convertire
 * 
 * @return l'argomento convertito a numero
 **/
static int convertLength(const char *argument) {
    char buffer[3] = {0};

    // La lunghezza della stringa deve essere esattamente 1
    strncpy(buffer, argument, 2);
    // Se la lunghezza della stringa è maggiore di 1 oppure è un carattere non compreso tra '1' e '8' (inclusi)...
    if (buffer[1] != 0 || buffer[0] < 49 || buffer[0] > 56)
        // ...stampa un messaggio di errore ed esci...
        exitInvalidNumber();       
    // ...altrimenti ottengo banalmente il numero
    return buffer[0] - 48;
}

/**
 * Converte un argormento in una codifica che rappresenti l'alfabeto da utilizzare.
 * 
 * @param argument argomento da convertire
 * @param alphabet array da riempire con i caratteri dell'alfabeto scelto
 * @param alphabetLength puntatore alla variabile da riempire con la dimensione dell'alfabeto
 **/
static void convertAlphabet(const char *argument, char *alphabet, int *alphabetLength) {
    char buffer[7] = {0};
    bool visited[5] = {false};
    int alphabetLengthTemp = 0, index = 0;
    bool isValid = true, argumentEnd = false;

    // La lunghezza della stringa deve essere al più 5
    strncpy(buffer, argument, 6);
    // Se la lunghezza della stringa è maggiore di 5...
    if (buffer[5] != 0)
        // ...stampa un messaggio di errore ed esci...
        exitAlphabetArgumentTooLong();
    // ...altrimenti controlla che la stringa sia ben formata:
    // - unicità dei caratteri: verificata dall'array dei visitati
    // - solo caratteri ammessi: verificata dallo switch-case
    for (int i = 0; i < 5 && isValid && !argumentEnd; i++)
        // Ogni volta che incontro una lettera della codifica
        // devo aggiornare la dimension finale dell'alfabeto
        switch (buffer[i]) {
            // Stringa terminata
            case 0:
                argumentEnd = true;
                break;
            // Codifica per le cifre
            case 'd':
                if (visited[0])
                    isValid = false;
                else {
                    visited[0] = true;
                    alphabetLengthTemp += 10;
                }
                break;
            // Codifica per le lettere minuscole
            case 'l':
                if (visited[1])
                    isValid = false;
                else {
                    visited[1] = true;
                    alphabetLengthTemp += 26;
                }
                break;
            // Codifica per le lettere maiuscole
            case 'u':
                if (visited[2])
                    isValid = false;
                else {
                    visited[2] = true;
                    alphabetLengthTemp += 26;
                }
                break;
            // Codifica per i simboli
            case 's':
                if (visited[3])
                    isValid = false;
                else {
                    visited[3] = true;
                    alphabetLengthTemp += 33;
                }
                break;
            //  Codifica per i caratteri di controllo
            case 'c':
                if (visited[4])
                    isValid = false;
                else {
                    visited[4] = true;
                    alphabetLengthTemp += 33;
                }
                break;
            // Codifica non valida
            default:
                isValid = false;
        }
    // Se la codifica è valida...
    if (!isValid)
        // ...stampa un messaggio di errore ed esci...
        exitInvalidAlphabet();
    // ...altrimenti controlla quali caratteri sono stati trovati e copia i relativi dati...
    if (visited[0]) {
        memcpy(alphabet + index, DIGITS, 10);
        index += 10;
    }
    if (visited[1]) {
        memcpy(alphabet + index, LOWER, 26);
        index += 26;
    }
    if (visited[2]) {
        memcpy(alphabet + index, UPPER, 26);
        index += 26;
    }
    if (visited[3]) {
        memcpy(alphabet + index, SYMBOLS, 33);
        index += 33;
    }
    if (visited[4]) {
        memcpy(alphabet + index, CONTROL, 33);
        index += 33;
    }
    *alphabetLength = alphabetLengthTemp;
}

/**
 * Inizializza la struttura con le informazioni in input.
 * 
 * @param argc numero di argomenti del programma
 * @param argv puntatore alla lista degli argomenti
 * 
 * @return la struttura inizializzata
 **/
InputDESCrack inputDESCrackInit(int argc, char **argv) {
    extern int optind, opterr, optopt;
    extern char *optarg;
    InputDESCrack input = {.minKeyLength = 1, .maxKeyLength = 8};
    int opt = 0;
    bool customMinKeyLength = false, customMaxKeyLength = false, customAlphabet = false,
         plainTextAsHex = false;

    opterr = 0;
    opt = getopt(argc, argv, "xm:M:a:");
    while (opt != -1) {
        // Ogni opzione non deve essere duplicata
        switch (opt) {
            case 'm':
                if (customMinKeyLength)
                    exitDuplicatedOption(opt);
                customMinKeyLength = true;
                input.minKeyLength = convertLength(optarg);
                break;
            case 'M':
                if (customMaxKeyLength)
                    exitDuplicatedOption(opt);
                customMaxKeyLength = true;
                input.maxKeyLength = convertLength(optarg);
                break;
            case 'a':
                if (customAlphabet)
                    exitDuplicatedOption(opt);
                customAlphabet = true;
                convertAlphabet(optarg, input.alphabet, &input.alphabetLength);
                break;
            case 'x':
                if (plainTextAsHex)
                    exitDuplicatedOption(opt);
                plainTextAsHex = true;
                break;
            // Opzione non valida
            default:
                exitInvalidOption(optopt);
        }
        opt = getopt(argc, argv, "xm:M:a:");
    }
    // Esci se non ci sono i due argomenti obbligatori
    if (optind + 2 > argc)
        exitMissingArgument();
    // Stessa cosa se ce sono troppi
    if (optind + 2 < argc)
        exitTooManyArguments();
    // Stessa cosa se min > max
    if (input.minKeyLength > input.maxKeyLength)
        exitMinGreaterThanMax();
    // Converte il primo argomento
    input.cipherTextBlock = convertHexArgument(argv[optind]);
    // Converte il secondo argomento, in base alla presenza dell'opzione
    if (!plainTextAsHex)
        input.plainTextBlock = convertTextArgument(argv[optind + 1]);
    else {
        input.plainTextBlock = convertHexArgument(argv[optind + 1]);
    }
    // Se non è stato scelto un alfabeto, ne viene utilizzato uno di default
    if (input.alphabetLength == 0)
        convertAlphabet("dlusc", input.alphabet, &input.alphabetLength);
    return input;
}