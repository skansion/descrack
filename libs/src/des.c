#include "des.h"


// Mappa che rappresenta le tabelle Sk del documento FIPS 46-3.
// I valori sono ricalcolati per evitare shift extra non necessari
static CONSTANT unsigned int S[8][64] = {{0xe0000000,          0, 0x40000000, 0xf0000000, 0xd0000000, 0x70000000, 0x10000000, 0x40000000,
                                          0x20000000, 0xe0000000, 0xf0000000, 0x20000000, 0xb0000000, 0xd0000000, 0x80000000, 0x10000000,
                                          0x30000000, 0xa0000000, 0xa0000000, 0x60000000, 0x60000000, 0xc0000000, 0xc0000000, 0xb0000000,
                                          0x50000000, 0x90000000, 0x90000000, 0x50000000,          0, 0x30000000, 0x70000000, 0x80000000,
                                          0x40000000, 0xf0000000, 0x10000000, 0xc0000000, 0xe0000000, 0x80000000, 0x80000000, 0x20000000,
                                          0xd0000000, 0x40000000, 0x60000000, 0x90000000, 0x20000000, 0x10000000, 0xb0000000, 0x70000000,
                                          0xf0000000, 0x50000000, 0xc0000000, 0xb0000000, 0x90000000, 0x30000000, 0x70000000, 0xe0000000,
                                          0x30000000, 0xa0000000, 0xa0000000,          0, 0x50000000, 0x60000000,          0, 0xd0000000},
                                         { 0xf000000,  0x3000000,  0x1000000,  0xd000000,  0x8000000,  0x4000000,  0xe000000,  0x7000000,
                                           0x6000000,  0xf000000,  0xb000000,  0x2000000,  0x3000000,  0x8000000,  0x4000000,  0xe000000,
                                           0x9000000,  0xc000000,  0x7000000,          0,  0x2000000,  0x1000000,  0xd000000,  0xa000000,
                                           0xc000000,  0x6000000,          0,  0x9000000,  0x5000000,  0xb000000,  0xa000000,  0x5000000,
                                                   0,  0xd000000,  0xe000000,  0x8000000,  0x7000000,  0xa000000,  0xb000000,  0x1000000,
                                           0xa000000,  0x3000000,  0x4000000,  0xf000000,  0xd000000,  0x4000000,  0x1000000,  0x2000000,
                                           0x5000000,  0xb000000,  0x8000000,  0x6000000,  0xc000000,  0x7000000,  0x6000000,  0xc000000,
                                           0x9000000,          0,  0x3000000,  0x5000000,  0x2000000,  0xe000000,  0xf000000,  0x9000000},
                                         {  0xa00000,   0xd00000,          0,   0x700000,   0x900000,          0,   0xe00000,   0x900000,
                                            0x600000,   0x300000,   0x300000,   0x400000,   0xf00000,   0x600000,   0x500000,   0xa00000,
                                            0x100000,   0x200000,   0xd00000,   0x800000,   0xc00000,   0x500000,   0x700000,   0xe00000,
                                            0xb00000,   0xc00000,   0x400000,   0xb00000,   0x200000,   0xf00000,   0x800000,   0x100000,
                                            0xd00000,   0x100000,   0x600000,   0xa00000,   0x400000,   0xd00000,   0x900000,          0,
                                            0x800000,   0x600000,   0xf00000,   0x900000,   0x300000,   0x800000,          0,   0x700000,
                                            0xb00000,   0x400000,   0x100000,   0xf00000,   0x200000,   0xe00000,  0xc00000,    0x300000,
                                            0x500000,   0xb00000,   0xa00000,   0x500000,   0xe00000,   0x200000,  0x700000,    0xc00000},
                                         {   0x70000,    0xd0000,    0xd0000,    0x80000,    0xe0000,    0xb0000,   0x30000,     0x50000,
                                                   0,    0x60000,    0x60000,    0xf0000,    0x90000,          0,   0xa0000,     0x30000,
                                             0x10000,    0x40000,    0x20000,    0x70000,    0x80000,    0x20000,   0x50000,     0xc0000,
                                             0xb0000,    0x10000,    0xc0000,    0xa0000,    0x40000,    0xe0000,   0xf0000,     0x90000,
                                             0xa0000,    0x30000,    0x60000,    0xf0000,    0x90000,          0,         0,     0x60000,
                                             0xc0000,    0xa0000,    0xb0000,    0x10000,    0x70000,    0xd0000,   0xd0000,     0x80000,
                                             0xf0000,    0x90000,    0x10000,    0x40000,    0x30000,    0x50000,   0xe0000,     0xb0000,
                                             0x50000,    0xc0000,    0x20000,    0x70000,    0x80000,    0x20000,   0x40000,     0xe0000},
                                         {    0x2000,     0xe000,     0xc000,     0xb000,     0x4000,     0x2000,    0x1000,      0xc000,
                                              0x7000,     0x4000,     0xa000,     0x7000,     0xb000,     0xd000,    0x6000,      0x1000,
                                              0x8000,     0x5000,     0x5000,          0,     0x3000,     0xf000,    0xf000,      0xa000,
                                              0xd000,     0x3000,          0,     0x9000,     0xe000,     0x8000,    0x9000,      0x6000,
                                              0x4000,     0xb000,     0x2000,     0x8000,     0x1000,     0xc000,    0xb000,      0x7000,
                                              0xa000,     0x1000,     0xd000,     0xe000,     0x7000,     0x2000,    0x8000,      0xd000,
                                              0xf000,     0x6000,     0x9000,     0xf000,     0xc000,          0,    0x5000,      0x9000,
                                              0x6000,     0xa000,     0x3000,     0x4000,          0,     0x5000,    0xe000,      0x3000},
                                         {     0xc00,      0xa00,      0x100,      0xf00,      0xa00,      0x400,     0xf00,       0x200,
                                               0x900,      0x700,      0x200,      0xc00,      0x600,      0x900,     0x800,       0x500,
                                                   0,      0x600,      0xd00,      0x100,      0x300,      0xd00,     0x400,       0xe00,
                                               0xe00,          0,      0x700,      0xb00,      0x500,      0x300,      0xb00,      0x800,
                                               0x900,      0x400,      0xe00,      0x300,      0xf00,      0x200,      0x500,      0xc00,
                                               0x200,      0x900,      0x800,      0x500,      0xc00,      0xf00,      0x300,      0xa00,
                                               0x700,      0xb00,          0,      0xe00,      0x400,      0x100,      0xa00,      0x700,
                                               0x100,      0x600,      0xd00,          0,      0xb00,      0x800,      0x600,      0xd00},
                                         {      0x40,       0xd0,       0xb0,          0,       0x20,       0xb0,       0xe0,       0x70,
                                                0xf0,       0x40,          0,       0x90,       0x80,       0x10,       0xd0,       0xa0,
                                                0x30,       0xe0,       0xc0,       0x30,       0x90,       0x50,       0x70,       0xc0,
                                                0x50,       0x20,       0xa0,       0xf0,       0x60,       0x80,       0x10,       0x60,
                                                0x10,       0x60,       0x40,       0xb0,       0xb0,       0xd0,       0xd0,       0x80,
                                                0xc0,       0x10,       0x30,       0x40,       0x70,       0xa0,       0xe0,       0x70,
                                                0xa0,       0x90,       0xf0,       0x50,       0x60,          0,       0x80,       0xf0,
                                                   0,       0xe0,       0x50,       0x20,       0x90,       0x30,       0x20,       0xc0},
                                         {       0xd,        0x1,        0x2,        0xf,        0x8,        0xd,        0x4,        0x8,
                                                 0x6,        0xa,        0xf,        0x3,        0xb,        0x7,        0x1,        0x4,
                                                 0xa,        0xc,        0x9,        0x5,        0x3,        0x6,        0xe,        0xb,
                                                 0x5,          0,          0,        0xe,        0xc,        0x9,        0x7,        0x2,
                                                 0x7,        0x2,        0xb,        0x1,        0x4,        0xe,        0x1,        0x7,
                                                 0x9,        0x4,        0xc,        0xa,        0xe,        0x8,        0x2,        0xd,
                                                   0,        0xf,        0x6,        0xc,        0xa,        0x9,        0xd,          0,
                                                 0xf,        0x3,        0x3,        0x5,        0x5,        0x6,        0x8,        0xb}};
// Mappa delle parità dispari di ogni byte tra 0 e 255
static const int ODDPARITY[256] = {  1,   1,   2,   2,   4,   4,   7,   7,   8,   8,  11,  11,  13,  13,  14,  14,
                                    16,  16,  19,  19,  21,  21,  22,  22,  25,  25,  26,  26,  28,  28,  31,  31,
                                    32,  32,  35,  35,  37,  37,  38,  38,  41,  41,  42,  42,  44,  44,  47,  47,
                                    49,  49,  50,  50,  52,  52,  55,  55,  56,  56,  59,  59,  61,  61,  62,  62,
                                    64,  64,  67,  67,  69,  69,  70,  70,  73,  73,  74,  74,  76,  76,  79,  79,
                                    81,  81,  82,  82,  84,  84,  87,  87,  88,  88,  91,  91,  93,  93,  94,  94,
                                    97,  97,  98,  98, 100, 100, 103, 103, 104, 104, 107, 107, 109, 109, 110, 110,
                                   112, 112, 115, 115, 117, 117, 118, 118, 121, 121, 122, 122, 124, 124, 127, 127,
                                   128, 128, 131, 131, 133, 133, 134, 134, 137, 137, 138, 138, 140, 140, 143, 143,
                                   145, 145, 146, 146, 148, 148, 151, 151, 152, 152, 155, 155, 157, 157, 158, 158,
                                   161, 161, 162, 162, 164, 164, 167, 167, 168, 168, 171, 171, 173, 173, 174, 174,
                                   176, 176, 179, 179, 181, 181, 182, 182, 185, 185, 186, 186, 188, 188, 191, 191,
                                   193, 193, 194, 194, 196, 196, 199, 199, 200, 200, 203, 203, 205, 205, 206, 206,
                                   208, 208, 211, 211, 213, 213, 214, 214, 217, 217, 218, 218, 220, 220, 223, 223,
                                   224, 224, 227, 227, 229, 229, 230, 230, 233, 233, 234, 234, 236, 236, 239, 239,
                                   241, 241, 242, 242, 244, 244, 247, 247, 248, 248, 251, 251, 253, 253, 254, 254};


/**
 * Funzione Permutation Choice 1, o PC1. 
 * Questa funzione è descritta all'interno del documento FIPS 46-3.
 * 
 * @param key puntatore alla chiave da utilizzare
 * 
 * @return il risultato di questa funzione, come descritto nel documento FIPS 46-3
 **/
DEVICE static DESBlock permutationChoice1(const DESBlock *key) {
    DESBlock block56 = {0, 0};

    // I bit sono raggruppati per tipo e valore dello shift
    block56.hi = (key->lo & 0x80) << 20 |
                 (key->lo & 0x8000) << 11 |
                 (key->lo & 0x800000) << 2 |
                 (key->lo & 0x80000000) >> 7 |
                 (key->hi & 0x80) << 16 |
                 (key->hi & 0x8000) << 7 |
                 (key->hi & 0x800000) >> 2 |
                 (key->hi & 0x80000000) >> 11 |
                 (key->lo & 0x40) << 13 |
                 (key->lo & 0x4000) << 4 |
                 (key->lo & 0x400000) >> 5 |
                 (key->lo & 0x40000000) >> 14 |
                 (key->hi & 0x40) << 9 |
                 (key->hi & 0x4000) |
                 (key->hi & 0x400000) >> 9 |
                 (key->hi & 0x40000000) >> 18 |
                 (key->lo & 0x20) << 6 |
                 (key->lo & 0x2000) >> 3 |
                 (key->lo & 0x200000) >> 12 |
                 (key->lo & 0x20000000) >> 21 |
                 (key->hi & 0x20) << 2 |
                 (key->hi & 0x2000) >> 7 |
                 (key->hi & 0x200000) >> 16 |
                 (key->hi & 0x20000000) >> 25 |
                 (key->lo & 0x10) >> 1 |
                 (key->lo & 0x1000) >> 10 |
                 (key->lo & 0x100000) >> 19 |
                 (key->lo & 0x10000000) >> 28;
    block56.lo = (key->lo & 0x2) << 26 |
                 (key->lo & 0x204) << 17 |
                 (key->lo & 0x20408) << 8 |
                 (key->lo & 0x2040800) >> 1 |
                 (key->hi & 0x2) << 22 |
                 (key->hi & 0x204) << 13 |
                 (key->hi & 0x20408) << 4 |
                 (key->hi & 0x2040800) >> 5 |
                 (key->lo & 0x4080000) >> 10 |
                 (key->hi & 0x4080000) >> 14 |
                 (key->lo & 0x8000000) >> 19 |
                 (key->hi & 0x8000000) >> 23 |
                 (key->hi & 0x10) >> 1 |
                 (key->hi & 0x1000) >> 10 |
                 (key->hi & 0x100000) >> 19 |
                 (key->hi & 0x10000000) >> 28;
    return block56;
}

/**
 * Funzione Permutation Choice 2, o PC2. 
 * Questa funzione è descritta all'interno del documento FIPS 46-3.
 * 
 * @param block56 puntatore al blocco da permutare
 * 
 * @return il risultato di questa funzione, come descritto nel documento FIPS 46-3
 **/
DEVICE static DESBlock permutationChoice2(const DESBlock *block56) {
    DESBlock block48 = {0, 0};

    // I bit sono raggruppati per tipo e valore dello shift
    block48.hi = (block56->hi & 0x4000) << 9 |
                 (block56->hi & 0x800) << 11 |
                 (block56->hi & 0x20000) << 4 |
                 (block56->hi & 0x11) << 16 |
                 (block56->hi & 0xa400000) >> 8 |
                 (block56->hi & 0x800000) >> 5 |
                 (block56->hi & 0x2002) << 2 |
                 (block56->hi & 0xa0) << 6 |
                 (block56->hi & 0x40100) >> 6 |
                 (block56->hi & 0x200) << 1 |
                 (block56->hi & 0x11000) >> 7 |
                 (block56->hi & 0x1000000) >> 16 |
                 (block56->hi & 0x4) << 5 |
                 (block56->hi & 0x108000) >> 14 |
                 (block56->hi & 0x200000) >> 17 |
                 (block56->hi & 0x4000000) >> 26;
    block48.lo = (block56->lo & 0x8001) << 8 |
                 (block56->lo & 0x10) << 18 |
                 (block56->lo & 0x2000000) >> 4 |
                 (block56->lo & 0x80000) << 1 |
                 (block56->lo & 0x220) << 10 |
                 (block56->lo & 0x2) << 17 |
                 (block56->lo & 0x4000000) >> 9 |
                 (block56->lo & 0x10000) |
                 (block56->lo & 0x888) << 3 |
                 (block56->lo & 0x804000) >> 10 |
                 (block56->lo & 0x100) << 4 |
                 (block56->lo & 0x1000) >> 1 |
                 (block56->lo & 0x20000) >> 8 |
                 (block56->lo & 0x400000) >> 15 |
                 (block56->lo & 0x400) >> 5 |
                 (block56->lo & 0x40) >> 3 |
                 (block56->lo & 0x100000) >> 18 |
                 (block56->lo & 0x8000000) >> 26 |
                 (block56->lo & 0x1000000) >> 24;
    return block48;
}

/**
 * Inizializzazione delle sottochiave, per la cifratura. 
 * Questa funzione è descritta all'interno del documento FIPS 46-3.
 * 
 * @param key puntatore alla chiave da utilizzare
 * @param keySchedule array da riempire con le sottochiavi
 **/
DEVICE static void keyScheduleInit(const DESBlock *key, DESBlock *keySchedule) {
    // C := block56.hi (valore a 32 bit, maschera necessaria)
    // D := block56.lo (valore a 32 bit, maschera necessaria)
    DESBlock block56 = permutationChoice1(key);

    // L'ultima sottochiave si può ottenere dalle due metà non ancora ruotate
    keySchedule[15] = permutationChoice2(&block56);
    for (int i = 0; i < 2; i++) {
        block56.hi = ((block56.hi << 1) & 0xfffffff) | (block56.hi >> 27);
        block56.lo = ((block56.lo << 1) & 0xfffffff) | (block56.lo >> 27);
        keySchedule[i] = permutationChoice2(&block56);
    }
    for (int i = 2; i < 8; i++) {
        block56.hi = ((block56.hi << 2) & 0xfffffff) | (block56.hi >> 26);
        block56.lo = ((block56.lo << 2) & 0xfffffff) | (block56.lo >> 26);
        keySchedule[i] = permutationChoice2(&block56);
    }
    block56.hi = ((block56.hi << 1) & 0xfffffff) | (block56.hi >> 27);
    block56.lo = ((block56.lo << 1) & 0xfffffff) | (block56.lo >> 27);
    keySchedule[8] = permutationChoice2(&block56);
    for (int i = 9; i < 15; i++) {
        block56.hi = ((block56.hi << 2) & 0xfffffff) | (block56.hi >> 26);
        block56.lo = ((block56.lo << 2) & 0xfffffff) | (block56.lo >> 26);
        keySchedule[i] = permutationChoice2(&block56);
    }
}

/**
 * Inizializzazione delle sottochiave, per la decifratura. 
 * Questa funzione è descritta all'interno del documento FIPS 46-3.
 * 
 * @param key puntatore alla chiave da utilizzare
 * @param keySchedule array da riempire con le sottochiavi
 **/
DEVICE static void reverseKeyScheduleInit(const DESBlock *key, DESBlock *keySchedule) {
    // C := block56.hi (valore a 32 bit, maschera necessaria)
    // D := block56.lo (valore a 32 bit, maschera necessaria)
    DESBlock block56 = permutationChoice1(key);

    // La prima sottochiave si può ottenere dalle due metà non ancora ruotate
    keySchedule[0] = permutationChoice2(&block56);
    for (int i = 0; i < 2; i++) {
        block56.hi = ((block56.hi << 1) & 0xfffffff) | (block56.hi >> 27);
        block56.lo = ((block56.lo << 1) & 0xfffffff) | (block56.lo >> 27);
        keySchedule[15 - i] = permutationChoice2(&block56);
    }
    for (int i = 2; i < 8; i++) {
        block56.hi = ((block56.hi << 2) & 0xfffffff) | (block56.hi >> 26);
        block56.lo = ((block56.lo << 2) & 0xfffffff) | (block56.lo >> 26);
        keySchedule[15 - i] = permutationChoice2(&block56);
    }
    block56.hi = ((block56.hi << 1) & 0xfffffff) | (block56.hi >> 27);
    block56.lo = ((block56.lo << 1) & 0xfffffff) | (block56.lo >> 27);   
    keySchedule[7] = permutationChoice2(&block56);
    for (int i = 9; i < 15; i++) {
        block56.hi = ((block56.hi << 2) & 0xfffffff) | (block56.hi >> 26);
        block56.lo = ((block56.lo << 2) & 0xfffffff) | (block56.lo >> 26);
        keySchedule[15 - i] = permutationChoice2(&block56);
    }
}

/**
 * Funzione Initial Permutation, o IP. 
 * Questa funzione è descritta all'interno del documento FIPS 46-3.
 * 
 * @param block64 puntatore al blocco in input
 * 
 * @return il risultato di questa funzione, come descritto nel documento FIPS 46-3
 **/
DEVICE static DESBlock initialPermutation(const DESBlock *block64) {
    DESBlock block64Perm = {0, 0};

    // I bit sono raggruppati per tipo e valore dello shift
    block64Perm.hi = (block64->lo & 0x40) << 25 |
                     (block64->lo & 0x4000) << 16 |
                     (block64->lo & 0x400001) << 7 |
                     (block64->lo & 0x40000100) >> 2 |
                     (block64->hi & 0x40) << 21 |
                     (block64->hi & 0x4000) << 12 |
                     (block64->hi & 0x400001) << 3 |
                     (block64->hi & 0x40000100) >> 6 |
                     (block64->lo & 0x10) << 19 |
                     (block64->lo & 0x1000) << 10 |
                     (block64->lo & 0x100000) << 1 |
                     (block64->lo & 0x10000000) >> 8 |
                     (block64->hi & 0x10) << 15 |
                     (block64->hi & 0x1000) << 6 |
                     (block64->hi & 0x100000) >> 3 |
                     (block64->hi & 0x10000000) >> 12 |
                     (block64->lo & 0x4) << 13 |
                     (block64->lo & 0x400) << 4 |
                     (block64->lo & 0x40000) >> 5 |
                     (block64->lo & 0x4000000) >> 14 |
                     (block64->hi & 0x4) << 9 |
                     (block64->hi & 0x400) |
                     (block64->hi & 0x40000) >> 9 |
                     (block64->hi & 0x4000000) >> 18 |
                     (block64->lo & 0x10000) >> 11 |
                     (block64->lo & 0x1000000) >> 20 |
                     (block64->hi & 0x10000) >> 15 |
                     (block64->hi & 0x1000000) >> 24;
    block64Perm.lo = (block64->lo & 0x80) << 24 |
                     (block64->lo & 0x8000) << 15 |
                     (block64->lo & 0x800002) << 6 |
                     (block64->lo & 0x80000200) >> 3 |
                     (block64->hi & 0x80) << 20 |
                     (block64->hi & 0x8000) << 11 |
                     (block64->hi & 0x800002) << 2 |
                     (block64->hi & 0x80000200) >> 7 |
                     (block64->lo & 0x20) << 18 |
                     (block64->lo & 0x2000) << 9 |
                     (block64->lo & 0x200000) |
                     (block64->lo & 0x20000000) >> 9 |
                     (block64->hi & 0x20) << 14 |
                     (block64->hi & 0x2000) << 5 |
                     (block64->hi & 0x200000) >> 4 |
                     (block64->hi & 0x20000000) >> 13 |
                     (block64->lo & 0x8) << 12 |
                     (block64->lo & 0x800) << 3 |
                     (block64->lo & 0x80000) >> 6 |
                     (block64->lo & 0x8000000) >> 15 |
                     (block64->hi & 0x8) << 8 |
                     (block64->hi & 0x800) >> 1 |
                     (block64->hi & 0x80000) >> 10 |
                     (block64->hi & 0x8000000) >> 19 |
                     (block64->lo & 0x20000) >> 12 |
                     (block64->lo & 0x2000000) >> 21 |
                     (block64->hi & 0x20000) >> 16 |
                     (block64->hi & 0x2000000) >> 25;
    return block64Perm;
}

/**
 * Funzione Expansion, o E
 * Questa funzione è descritta all'interno del documento FIPS 46-3.
 * 
 * @param block32 valore in input per la funzione di cifratura
 * 
 * @return il risultato di questa funzione, come descritto nel documento FIPS 46-3
 **/
DEVICE static DESBlock expansionFunction(const unsigned int block32) {
    DESBlock block48 = {0, 0};

    // I bit sono raggruppati per tipo e valore dello shift
    block48.hi = (block32 & 0x1) << 23 |
                 (block32 & 0xf8000000) >> 9 |
                 (block32 & 0x1f800000) >> 11 |
                 (block32 & 0x1f80000) >> 13 |
                 (block32 & 0x1f8000) >> 15;
    block48.lo = (block32 & 0x1f800) << 7 |
                 (block32 & 0x1f80) << 5 |
                 (block32 & 0x1f8) << 3 |
                 (block32 & 0x1f) << 1 |
                 (block32 & 0x80000000) >> 31;
    return block48;
}

/**
 * Funzione Selection, o S
 * Questa funzione è descritta all'interno del documento FIPS 46-3.
 * 
 * @param block48 puntatore al blocco ottenuto dalla funzione di espansione
 * 
 * @return il risultato di questa funzione, come descritto nel documento FIPS 46-3
 **/
DEVICE static unsigned int selectionFunction(const DESBlock *block48) {
    return S[0][(block48->hi & 0xfc0000) >> 18] |
           S[1][(block48->hi & 0x3f000) >> 12] |
           S[2][(block48->hi & 0xfc0) >> 6] |
           S[3][(block48->hi & 0x3f)] |
           S[4][(block48->lo & 0xfc0000) >> 18] |
           S[5][(block48->lo & 0x3f000) >> 12] |
           S[6][(block48->lo & 0xfc0) >> 6] |
           S[7][(block48->lo & 0x3f)];
}

/**
 * Funzione Permutation, o P
 * Questa funzione è descritta all'interno del documento FIPS 46-3.
 * 
 * @param block32 valore ottenuto dalla funzione di selezione
 * 
 * @return il risultato di questa funzione, come descritto nel documento FIPS 46-3
 **/
DEVICE static unsigned int permutationFunction(const unsigned int block32) {
    // I bit sono raggruppati per tipo e valore dello shift
    return (block32 & 0x10000) << 15 |
           (block32 & 0x2020120) << 5 |
           (block32 & 0x1800) << 17 |
           (block32 & 0x8) << 24 |
           (block32 & 0x100000) << 6 |
           (block32 & 0x10) << 21 |
           (block32 & 0x8000) << 9 |
           (block32 & 0x88000000) >> 8 |
           (block32 & 0x200) << 12 |
           (block32 & 0x40) << 14 |
           (block32 & 0x4000) << 4 |
           (block32 & 0x2) << 16 |
           (block32 & 0x442000) >> 6 |
           (block32 & 0x40800000) >> 15 |
           (block32 & 0x1000000) >> 10 |
           (block32 & 0x1) << 11 |
           (block32 & 0x20000000) >> 20 |
           (block32 & 0x80000) >> 13 |
           (block32 & 0x4) << 3 |
           (block32 & 0x4000000) >> 22 |
           (block32 & 0x480) >> 7 |
           (block32 & 0x200000) >> 19 |
           (block32 & 0x10000000) >> 27;
}

/**
 * Funzione di cifratura
 * Questa funzione è descritta all'interno del documento FIPS 46-3.
 * 
 * @param block32 valore della metà destra di un blocco
 * @param keyScheduleK puntatore alla k-esima sottochiave
 * 
 * @return il risultato di questa funzione, come descritto nel documento FIPS 46-3
 **/
DEVICE static unsigned int cipherFunction(const unsigned int block32, const DESBlock *keyScheduleK) {
    DESBlock block48 = expansionFunction(block32);
    
    block48.hi ^= keyScheduleK->hi;
    block48.lo ^= keyScheduleK->lo;
    return permutationFunction(selectionFunction(&block48));
}

/**
 * Funzione Inverse Initial Permutation, o IP^-1. 
 * Questa funzione è descritta all'interno del documento FIPS 46-3.
 * 
 * @param block64 puntatore al blocco da permutare
 * 
 * @return il risultato di questa funzione, come descritto nel documento FIPS 46-3
 **/
DEVICE static DESBlock inverseInitialPermutation(const DESBlock *block64) {
    DESBlock block64InvPerm = {0, 0};

    // I bit sono raggruppati per tipo e valore dello shift
    block64InvPerm.hi = (block64->lo & 0x1000004) << 7 |
                        (block64->hi & 0x1000004) << 6 |
                        (block64->lo & 0x10000) << 13 |
                        (block64->hi & 0x10000) << 12 |
                        (block64->lo & 0x100) << 19 |
                        (block64->hi & 0x100) << 18 |
                        (block64->lo & 0x1) << 25 |
                        (block64->hi & 0x1) << 24 |
                        (block64->lo & 0x2000008) >> 2 |
                        (block64->hi & 0x2000008) >> 3 |
                        (block64->lo & 0x20000) << 4 |
                        (block64->hi & 0x20000) << 3 |
                        (block64->lo & 0x200) << 10 |
                        (block64->hi & 0x200) << 9 |
                        (block64->lo & 0x2) << 16 |
                        (block64->hi & 0x2) << 15 |
                        (block64->lo & 0x4000000) >> 11 |
                        (block64->hi & 0x4000000) >> 12 |
                        (block64->lo & 0x40000) >> 5 |
                        (block64->hi & 0x40000) >> 6 |
                        (block64->lo & 0x400) << 1 |
                        (block64->hi & 0x400) |
                        (block64->lo & 0x8000000) >> 20 |
                        (block64->hi & 0x8000000) >> 21 |
                        (block64->lo & 0x80000) >> 14 |
                        (block64->hi & 0x80000) >> 15 |
                        (block64->lo & 0x800) >> 8 |
                        (block64->hi & 0x800) >> 9;
    block64InvPerm.lo = (block64->lo & 0x10000040) << 3 |
                        (block64->hi & 0x10000040) << 2 |
                        (block64->lo & 0x100000) << 9 |
                        (block64->hi & 0x100000) << 8 |
                        (block64->lo & 0x1000) << 15 |
                        (block64->hi & 0x1000) << 14 |
                        (block64->lo & 0x10) << 21 |
                        (block64->hi & 0x10) << 20 |
                        (block64->lo & 0x20000080) >> 6 |
                        (block64->hi & 0x20000080) >> 7 |
                        (block64->lo & 0x200000) |
                        (block64->hi & 0x200000) >> 1 |
                        (block64->lo & 0x2000) << 6 |
                        (block64->hi & 0x2000) << 5 |
                        (block64->lo & 0x20) << 12 |
                        (block64->hi & 0x20) << 11 |
                        (block64->lo & 0x40000000) >> 15 |
                        (block64->hi & 0x40000000) >> 16 |
                        (block64->lo & 0x400000) >> 9 |
                        (block64->hi & 0x400000) >> 10 |
                        (block64->lo & 0x4000) >> 3 |
                        (block64->hi & 0x4000) >> 4 |
                        (block64->lo & 0x80000000) >> 24 |
                        (block64->hi & 0x80000000) >> 25 |
                        (block64->lo & 0x800000) >> 18 |
                        (block64->hi & 0x800000) >> 19 |
                        (block64->lo & 0x8000) >> 12 |
                        (block64->hi & 0x8000) >> 13;
    return block64InvPerm;
}

/**
 * Algoritmo DES. 
 * L'algoritmo è descritto all'interno del documento FIPS 46-3.
 * 
 * @param block64 puntatore al blocco in input (testo in chiaro o blocco cifrato)
 * @param keySchedule array contenente le sottochiavi
 * 
 * @return il blocco in output (blocco cifrato o testo in chiaro)
 **/
DEVICE static DESBlock des(const DESBlock *block64, const DESBlock *keySchedule) {
    DESBlock block64Temp = initialPermutation(block64);
    unsigned int block32Left = 0, block32Right = 0;
    
    for (int i = 0; i < 16; i++) {
        block32Right = block64Temp.lo;
        block32Left = cipherFunction(block32Right, keySchedule + i);
        block64Temp.lo = block64Temp.hi ^ block32Left;
        block64Temp.hi = block32Right;
    }
    block32Left = block64Temp.lo;
    block64Temp.lo = block64Temp.hi;
    block64Temp.hi = block32Left;
    return inverseInitialPermutation(&block64Temp);
}

/**
 * Cifra un testo in chiaro tramite una chiave.
 * 
 * @param plainTextBlock puntatore al testo in chiaro da cifrare
 * @param key puntatore alla chiave da utilizzare
 * 
 * @return il blocco cifrato
 **/
DEVICE DESBlock encrypt(const DESBlock *plainTextBlock, const DESBlock *key) {
    DESBlock keySchedule[16] = {{0, 0}};

    // Le sottochiavi vengono generate tutte insieme, per evitare chiamate inutili
    keyScheduleInit(key, keySchedule);
    return des(plainTextBlock, keySchedule);
}

/**
 * Decifra un blocco tramite una chiave.
 * 
 * @param cipherTextBlock puntatore al blocco da decifrare
 * @param key puntatore alla chiave da utilizzare
 * 
 * @return il testo in chiaro
 **/
DEVICE DESBlock decrypt(const DESBlock *cipherTextBlock, const DESBlock *key) {
    DESBlock keySchedule[16] = {{0, 0}};

    // Le sottochiavi vengono generate tutte insieme, per evitare chiamate inutili
    reverseKeyScheduleInit(key, keySchedule);
    return des(cipherTextBlock, keySchedule);
}

/**
 * Forza la chiave ad avere i bit di parità (dispari), come indicato nel documento FIPS 46-3
 * 
 * @param key puntatore alla chiave da convertire
 **/
void forceOddParity(DESBlock *key) {
    // Uso la mappa precalcolata
    key->hi = ODDPARITY[(key->hi & 0xff000000) >> 24] << 24 |
              ODDPARITY[(key->hi & 0xff0000) >> 16] << 16 |
              ODDPARITY[(key->hi & 0xff00) >> 8] << 8 |
              ODDPARITY[key->hi & 0xff];
    key->lo = ODDPARITY[(key->lo & 0xff000000) >> 24] << 24 |
              ODDPARITY[(key->lo & 0xff0000) >> 16] << 16 |
              ODDPARITY[(key->lo & 0xff00) >> 8] << 8 |
              ODDPARITY[key->lo & 0xff];
}