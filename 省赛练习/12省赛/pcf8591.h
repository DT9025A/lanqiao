#ifndef __PCF8591_H_
#define __PCF8591_H_

#include <STC15.H>
#include <intrins.h>

#include "iic.h"

void PCF8591_Config(unsigned char ctrl, unsigned char dac);
unsigned char PCF8591_Read();


#endif

