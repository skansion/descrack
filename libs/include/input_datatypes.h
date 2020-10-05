#ifndef __INPUT_DATATYPES_H__
#define __INPUT_DATATYPES_H__


#include "des_datatypes.h"


/**
 * Struttura che rappresenta l'input di DESCrack
 * 
 * @param cipherTextBlock il blocco cifrato da decifrare
 * @param plainTextBlock il testo in chiaro da confrontare
 * @param minKeyLength la lunghezza minima della chiave da cercare
 * @param maxKeyLength la lunghezza massima della chiave da cercare
 * @param alphabetLength la dimensione dell'alfabeto da utilizzare
 * @param alphabet l'array che rappresenta l'alfabeto da utilizzare
 **/
typedef struct InputDESCrack {
    DESBlock cipherTextBlock, plainTextBlock;
    int minKeyLength, maxKeyLength, alphabetLength;
    char alphabet[128];
} InputDESCrack;

/**
 * Struttura che rappresenta l'input di DESEncrypt
 * 
 * @param plainTextBlock il testo in chiaro da cifrare (come blocco DES)
 * @param key la chiave da utilizzare
 **/
typedef struct InputDESEncrypt {
    DESBlock plainTextBlock, key;
} InputDESEncrypt;


#endif