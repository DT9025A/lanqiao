#ifndef __PCF_H_
#define __PCF_H_

#include <STC15.H>
#include <intrins.h>
#include "iic.h"

void PCF8591_Init (unsigned char channel);
unsigned char PCF8591_Read();

#endif