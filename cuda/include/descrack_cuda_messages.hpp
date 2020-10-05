#ifndef __DESCRACK_CUDA_MESSAGES_HPP__
#define __DESCRACK_CUDA_MESSAGES_HPP__


#include <string>


void exitDuringDeviceInit(const std::string &reason);
void exitDuringCopytoDevice(const std::string &name, const std::string &reason);
void exitDuringKernelLaunch(const std::string &reason);
void exitDuringDeviceSynch(const std::string &reason);
void exitDuringCopyFromDevice(const std::string &name, const std::string &reason);


#endif