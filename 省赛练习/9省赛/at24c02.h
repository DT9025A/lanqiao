#ifndef __AT24C02_H_
#define __AT24C02_H_

#include <STC15.H>
#include "iic.h"

void WriteEEPROM(unsigned char add, unsigned char dat);
unsigned char ReadEEPROM(unsigned char add);

#endif
