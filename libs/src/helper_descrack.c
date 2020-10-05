#include "des.h"
#include "helper_descrack.h"


/**
 * Converte un valore di tipo long ad una chiave specifica.
 * 
 * @param keyLength lunghezza della chiave
 * @param value valore da convertire
 * @param pows array contenente le potenze da 0 da 8 della dimensione dell'alfabeto
 * @param alphabet array che rappresenta l'alfabeto da utilizzare
 * 
 * @return la chiave relativa al valore in input
 **/
DESBlock valueToKey(const int keyLength, const long value, const long *pows, const char *alphabet) {
    DESBlock key = {0, 0};

    // È possibile utilizzare uno switch-case con un solo break alla fine
    // perchè ogni parte in basso è richiesta da quelle in alto
    switch (keyLength) {
        case 8:
            key.lo |= alphabet[(value % pows[8]) / pows[7]] << 1;
        case 7:
            key.lo |= alphabet[(value % pows[7]) / pows[6]] << 9;
        case 6:
            key.lo |= alphabet[(value % pows[6]) / pows[5]] << 17;
        case 5:
            key.lo |= alphabet[(value % pows[5]) / pows[4]] << 25;
        case 4:
            key.hi |= alphabet[(value % pows[4]) / pows[3]] << 1;
        case 3:
            key.hi |= alphabet[(value % pows[3]) / pows[2]] << 9;
        case 2:
            key.hi |= alphabet[(value % pows[2]) / pows[1]] << 17;
        default :
            key.hi |= alphabet[(value % pows[1]) / pows[0]] << 25;
    }
    return key;
}

/**
 * Verifica che la chiave sia quella da utilizzare per decifrare il blocco. 
 * Richiede il testo in chiaro originale.
 * 
 * @param cipherTextBlock puntatore al blocco cifrato da utilizzare
 * @param plainTextBlock puntatore al testo in chiare da utilizzare
 * @param key puntatore alla chiave da testare
 * 
 * @return TRUE se la chiave è quella utilizzata per cifrare, FALSE altrimenti
 **/
bool keyTest(const DESBlock *cipherTextBlock, const DESBlock *plainTextBlock, const DESBlock *key) {
    DESBlock plainTextBlockTemp = decrypt(cipherTextBlock, key);

    return plainTextBlock->hi == plainTextBlockTemp.hi &&
           plainTextBlock->lo == plainTextBlockTemp.lo;
}