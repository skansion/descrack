#ifndef __HELPER_DESCRACK_H__
#define __HELPER_DESCRACK_H__


#include <stdbool.h>

#include "des_datatypes.h"


DESBlock valueToKey(const int keyLength, const long value, const long *pows, const char *alphabet);
bool keyTest(const DESBlock *cipherTextBlock, const DESBlock *plainTextBlock, const DESBlock *key);


#endif