#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <iomanip>
#include <iostream>

#include "des.h"
#include "descrack_cuda_messages.hpp"
#include "input_descrack.h"


// Valori costanti per ogni kernel
static __constant__ DESBlock cipherTextBlockDevice = {0, 0};
static __constant__ DESBlock plainTextBlockDevice = {0, 0};
static __constant__ char alphabetDevice[128] = {0};
static __constant__ int alphabetLengthDevice = 1;
// Valori "restituiti" dal kernel
static __device__ bool keyFoundDevice = false;
static __device__ DESBlock keyDevice = {0, 0};


/**
 * Converte un valore ad una chiave specifica. 
 * Su CUDA, il valore utilizza un intero per il primo byte e 
 * gli identificatori per thread/blocco/griglia per gli altri.
 * 
 * @param keyLength lunghezza della chiave
 * @param value valore da convertire nel primo byte della chiave
 * 
 * @return la chiave relativa al valore in input e agli identificatori
 **/
__device__ static DESBlock valueToKey(const int keyLength, const int value) {
    DESBlock key = {0, 0};

    // È possibile utilizzare uno switch-case con un solo break alla fine
    // perchè ogni parte in basso è richiesta da quelle in alto
    switch (keyLength) {
        case 8:
            key.lo |= alphabetDevice[blockIdx.z / alphabetLengthDevice] << 1;
        case 7:
            key.lo |= alphabetDevice[blockIdx.y / alphabetLengthDevice] << 9;
        case 6:
            key.lo |= alphabetDevice[blockIdx.x / alphabetLengthDevice] << 17;
        case 5:
            key.lo |= alphabetDevice[blockIdx.z % alphabetLengthDevice] << 25;
        case 4:
            key.hi |= alphabetDevice[blockIdx.y % alphabetLengthDevice] << 1;
        case 3:
            key.hi |= alphabetDevice[blockIdx.x % alphabetLengthDevice] << 9;
        case 2:
            key.hi |= alphabetDevice[threadIdx.x] << 17;
        default:
            key.hi |= alphabetDevice[value] << 25;
    }
    return key;
}

/**
 * Verifica che la chiave sia quella da utilizzare per decifrare il blocco. 
 * Su CUDA, tutti gli altri valori necessari per la verifica risiedono nella memoria costante del device.
 * 
 * @param key riferimento alla chiave da testare
 * 
 * @return TRUE se la chiave è quella utilizzata per cifrare, FALSE altrimenti
 **/
__device__ static bool keyTest(const DESBlock &key) {
    DESBlock plainTextBlockTemp = decrypt(&cipherTextBlockDevice, &key);

    return plainTextBlockDevice.hi == plainTextBlockTemp.hi &&
           plainTextBlockDevice.lo == plainTextBlockTemp.lo;
}

/**
 * Kernel CUDA.
 * 
 * @param keyLength lunghezza della chiave da utilizzare per generare le chiavi
 **/
__global__ static void crackKey(const int keyLength) {
    DESBlock keyTemp = {0, 0};

    for (int i = 0; i < alphabetLengthDevice && !keyFoundDevice; i++) {
        keyTemp = valueToKey(keyLength, i);
        if (keyTest(keyTemp)) {
            keyFoundDevice = true;
            keyDevice = keyTemp;
        }
    }
}

int main(int argc, char **argv) {
    InputDESCrack input = inputDESCrackInit(argc, argv);
    DESBlock key = {0, 0};
    bool keyFound = false;
    dim3 gridSize = 1;
    int blockSize = 1;
    cudaError_t error;

    // Controllo errori sul device, prima di iniziare
    error = cudaGetLastError();
    if (error != cudaSuccess)
        exitDuringDeviceInit(cudaGetErrorString(error));
    // Copia i valori nella memoria costante
    // In caso di errore, stampa un messaggio ed esci
    // NOTA: CUDA permette di assegnare/leggere valori nella memoria del device tramite il loro "simbolo",
    //  cioè il nome la variabile, che deve risiedere nella stessa unità di compilazione
    error = cudaMemcpyToSymbol(cipherTextBlockDevice, &input.cipherTextBlock, sizeof(DESBlock));
    if (error != cudaSuccess)
        exitDuringCopytoDevice("cipher text block", cudaGetErrorString(error));
    error = cudaMemcpyToSymbol(plainTextBlockDevice, &input.plainTextBlock, sizeof(DESBlock));
    if (error != cudaSuccess)
        exitDuringCopytoDevice("plain text block", cudaGetErrorString(error));
    error = cudaMemcpyToSymbol(alphabetLengthDevice, &input.alphabetLength, sizeof(int));
    if (error != cudaSuccess)
        exitDuringCopytoDevice("alphabet length value", cudaGetErrorString(error));
    error = cudaMemcpyToSymbol(alphabetDevice, &input.alphabet, input.alphabetLength * sizeof(char));
    if (error != cudaSuccess)
        exitDuringCopytoDevice("alphabet array", cudaGetErrorString(error));
    // Se la chiave non è stata ancora trovata, ripeti per ogni valore della lunghezza della chiave...
    for (int i = input.minKeyLength; i <= input.maxKeyLength && !keyFound; i++) {
        // Imposta le dimensioni di blocco e griglia
        switch (i) {
            case 8:
                gridSize.z = input.alphabetLength * input.alphabetLength;
            case 7:
                gridSize.y = input.alphabetLength * input.alphabetLength;
            case 6:
                gridSize.x = input.alphabetLength * input.alphabetLength;
            case 5:
                gridSize.z = gridSize.z == 1 ? input.alphabetLength : gridSize.z;
            case 4:
                gridSize.y = gridSize.y == 1 ? input.alphabetLength : gridSize.y;
            case 3:
                gridSize.x = gridSize.x == 1 ? input.alphabetLength : gridSize.x;
            case 2:
                blockSize = input.alphabetLength;
            default:
                break;
        }
        // Lancia il kernel
        crackKey<<<gridSize, blockSize>>>(i);
        // Controllo errori durante il lancio del kernel
        error = cudaGetLastError();
        if (error != cudaSuccess)
            exitDuringKernelLaunch(cudaGetErrorString(error));
        // Attendo il termine dell'esecuzione e controllo eventuali errori
        error = cudaDeviceSynchronize();
        if (error != cudaSuccess)
            exitDuringDeviceSynch(cudaGetErrorString(error));
        // Copia il valore del flag dal device
        // In caso di errore, stampa un messaggio ed esci
        error = cudaMemcpyFromSymbol(&keyFound, keyFoundDevice, sizeof(bool));
        if (error != cudaSuccess)
            exitDuringCopyFromDevice("\"found\" flag", cudaGetErrorString(error));
        // Se la chiave è stata trovata...
        if (keyFound) {
            // ...copia il valore della chiave dal device
            // In caso di errore, stampa un messaggio ed esci
            error = cudaMemcpyFromSymbol(&key, keyDevice, sizeof(DESBlock));
            if (error != cudaSuccess)
                exitDuringCopyFromDevice("key found", cudaGetErrorString(error));
            // ...forza la parità (dispari) e stampala
            forceOddParity(&key);
            std::cout << std::setw(8) << std::setfill('0') << std::hex << key.hi
                      << std::setw(8) << std::setfill('0') << std::hex << key.lo
                      << std::endl;
        }
    }
    return EXIT_SUCCESS;
}