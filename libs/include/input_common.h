#ifndef __INPUT_COMMON_H__
#define __INPUT_COMMON_H__


#include "des_datatypes.h"


DESBlock convertTextArgument(const char *argument);
DESBlock convertHexArgument(const char *argument);


#endif