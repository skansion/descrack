#include <unistd.h>

#include "input_common.h"
#include "input_desencrypt.h"
#include "input_messages.h"


/**
 * Converte un argomento di tipo TEXT (stringa di caratteri ASCII) in un blocco DES. 
 * Questa versione è specifica per la chiave.
 * 
 * @param argument argomento da convertire
 * @param shiftTextKey flag per abilitare lo shift della chiave risultante
 * 
 * @return l'argomento convertito a blocco DES
 **/
static DESBlock convertTextKeyArgument(const char *argument, const bool shiftTextKey) {
    DESBlock key = convertTextArgument(argument);

    if (shiftTextKey) {
        key.hi <<= 1;
        key.lo <<= 1;
    }
    return key;
}

/**
 * Inizializza la struttura con le informazioni in input.
 * 
 * @param argc numero di argomenti del programma
 * @param argv puntatore alla lista degli argomenti
 * 
 * @return la struttura inizializzata
 **/
InputDESEncrypt inputDESEncryptInit(int argc, char **argv) {
    extern int optind, opterr, optopt;
    extern char *optarg;
    InputDESEncrypt input = {.key = {0, 0}};
    int opt = 0;
    bool plainTextAsHex = false, keyAsHex = false, shiftTextKey = false;

    opterr = 0;
    opt = getopt(argc, argv, "xXs");
    while (opt != -1) {
        // Ogni opzione non deve essere duplicata
        switch (opt) {
            case 'x':
                if (plainTextAsHex)
                    exitDuplicatedOption(opt);
                plainTextAsHex = true;
                break;
            case 'X':
                if (keyAsHex)
                    exitDuplicatedOption(opt);
                if (shiftTextKey)
                    exitIncompatibleFlags();
                keyAsHex = true;
                break;
            case 's':
                if (shiftTextKey)
                    exitDuplicatedOption(opt);
                if (keyAsHex)
                    exitIncompatibleFlags();
                shiftTextKey = true;
                break;
            // Opzione non valida
            default:
                exitInvalidOption(optopt);
        }
        opt = getopt(argc, argv, "xXs");
    }
    // Esci se non ci sono i due argomenti obbligatori
    if (optind + 2 > argc)
        exitMissingArgument();
    // Stessa cosa se ce sono troppi
    if (optind + 2 < argc)
        exitTooManyArguments();
    // Converte il primo argomento, in base alla presenza dell'opzione
    if (plainTextAsHex)
        input.plainTextBlock = convertHexArgument(argv[optind]);
    else
        input.plainTextBlock = convertTextArgument(argv[optind]);
    // Converte il secondo argomento, in base alla presenza delle opzioni
    if (keyAsHex)
        input.key = convertHexArgument(argv[optind + 1]);
    else
        input.key = convertTextKeyArgument(argv[optind + 1], shiftTextKey);
    return input;
}