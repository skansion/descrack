#ifndef __DES_H__
#define __DES_H__


// This code must be able to be compiled, even if CUDA is not present
#ifndef __CUDA_ARCH__
#define CONSTANT
#define DEVICE
#else
#define CONSTANT __constant__
#define DEVICE __device__


#include <cuda_runtime.h>
#endif


#include "des_datatypes.h"


DEVICE DESBlock encrypt(const DESBlock *plainTextBlock, const DESBlock *key);
DEVICE DESBlock decrypt(const DESBlock *cipherTextBlock, const DESBlock *key);
void forceOddParity(DESBlock *key);


#endif