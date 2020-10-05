#ifndef __DES_DATATYPES_H__
#define __DES_DATATYPES_H__


/**
 * Struttura che rappresenta un blocco DES. 
 * È utilizzato anche per memorizzare il testo in chiaro e la chiave
 * 
 * @param hi i 32 bit più significativi
 * @param lo i 32 bit meno significativi
 **/
typedef struct DESBlock {
   unsigned int hi, lo;
} DESBlock;


#endif