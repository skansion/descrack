#include <cstdlib>
#include <iostream>

#include "descrack_cuda_messages.hpp"


// Messaggi di errore
static const std::string DEVICEINITERR = "Error during device init: ";
static const std::string COPYERR = "Error during the copy of ";
static const std::string KERNELLAUNCHERR = "Error during the kernel launch: ";
static const std::string DEVICESYNCHERR = "Error during the device synch: ";
static const std::string TODEVICESUFFIX = " to device: ";
static const std::string FROMDEVICESUFFIX = " from device: ";


/**
 * Stampa un messaggio su STDERR ed esci.
 * 
 * @param message messaggio da stampare
 **/
static void exitCudaWithMessage(const std::string &message) {
    std::cerr << message << std::endl;
    std::exit(EXIT_FAILURE);
}

/**
 * Stampa un messaggio specifico su STDERR ed esci.
 * 
 * @param reason stringa indicante la motivazione dell'errore
 **/
void exitDuringDeviceInit(const std::string &reason) {
    exitCudaWithMessage(DEVICEINITERR + reason);
}

/**
 * Stampa un messaggio specifico su STDERR ed esci.
 * 
 * @param name stringa relativa alla posizione dell'errore
 * @param reason stringa relativa alla motivazione dell'errore
 **/
void exitDuringCopytoDevice(const std::string &name, const std::string &reason) {
    exitCudaWithMessage(COPYERR + name + TODEVICESUFFIX + reason);
}

/**
 * Stampa un messaggio specifico su STDERR ed esci.
 * 
 * @param reason stringa indicante la motivazione dell'errore
 **/
void exitDuringKernelLaunch(const std::string &reason) {
    exitCudaWithMessage(KERNELLAUNCHERR + reason);
}

/**
 * Stampa un messaggio specifico su STDERR ed esci.
 * 
 * @param reason stringa indicante la motivazione dell'errore
 **/
void exitDuringDeviceSynch(const std::string &reason) {
    exitCudaWithMessage(DEVICESYNCHERR + reason);
}

/**
 * Stampa un messaggio specifico su STDERR ed esci.
 * 
 * @param name stringa relativa alla posizione dell'errore
 * @param reason stringa relativa alla motivazione dell'errore
 **/
void exitDuringCopyFromDevice(const std::string &name, const std::string &reason) {
    exitCudaWithMessage(COPYERR + name + FROMDEVICESUFFIX + reason);
}