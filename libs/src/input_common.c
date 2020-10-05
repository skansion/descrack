#include <stdbool.h>
#include <string.h>

#include "input_common.h"
#include "input_messages.h"


/**
 * Converte un argomento di tipo TEXT (stringa di caratteri ASCII) in un blocco DES.
 * 
 * @param argument argomento da convertire
 * 
 * @return l'argomento convertito a blocco DES
 **/
DESBlock convertTextArgument(const char *argument) {
    DESBlock block = {0, 0};
    char buffer[10] = {0};

    // La grandezza della chiave è al più 64 bit (8 caratteri ASCII)
    strncpy(buffer, argument, 9);
    // Se la lunghezza della stringa è maggiore di 8...
    if (buffer[8] != 0)
        // ...stampa un messaggio di errore ed esci...
        exitTextArgumentTooLong();
    // ...altrimenti riempi il blocco (con eventuale padding)
    block.hi = buffer[0] << 24 |
               buffer[1] << 16 |
               buffer[2] << 8 |
               buffer[3];
    block.lo = buffer[4] << 24 |
               buffer[5] << 16 |
               buffer[6] << 8 |
               buffer[7];
    return block;
}

/**
 * Converte un argomento di tipo HEX (stringa di cifre esadecimali) in un blocco DES.
 * 
 * @param argument argomento da convertire
 * 
 * @return l'argomento convertito a blocco DES
 **/
DESBlock convertHexArgument(const char *argument) {
    DESBlock block = {0, 0};
    char buffer[18] = {0};
    bool isValid = true;

    // La grandezza della chiave è al più 64 bit (16 cifre esadecimali)
    strncpy(buffer, argument, 17);
    // Se la lunghezza della stringa è maggiore di 16...
    if (buffer[16] != 0)
        // ...stampa un messaggio di errore ed esci...
        exitHexArgumentTooLong();
    // ...altrimenti controlla che ogni carattere sia una cifra esadecimale
    for (int i = 0; i < 16 && isValid; i++)
        if (buffer[i] >= 48 && buffer[i] <= 57)
            buffer[i] -= 48;
        else
            if (buffer[i] >= 65 && buffer[i] <= 70)
                buffer[i] -= 55;
            else
                if (buffer[i] >= 97 && buffer[i] <= 102)
                    buffer[i] -= 87;
                else {
                    // Possibile carattere non valido
                    // Se '\0' la stringa è terminata, altrimenti non è valida
                    isValid = buffer[i] == 0;
                    break;
                }
    // Se la stringa non è un numero esadecimale...
    if (!isValid)
        // ...stampa un messaggio di errore ed esci...
        exitInvalidHexDigit();
    // ...altrimenti riempi il blocco (con eventuale padding)
    block.hi = buffer[0] << 28 |
               buffer[1] << 24 |
               buffer[2] << 20 |
               buffer[3] << 16 |
               buffer[4] << 12 |
               buffer[5] << 8 |
               buffer[6] << 4 |
               buffer[7];
    block.lo = buffer[8] << 28 |
               buffer[9] << 24 |
               buffer[10] << 20 |
               buffer[11] << 16 |
               buffer[12] << 12 |
               buffer[13] << 8 |
               buffer[14] << 4 |
               buffer[15];        
    return block;
}