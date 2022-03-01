#ifndef __ADC_H_
#define __ADC_H_

#include <STC15.H>
#include "iic.h"

void InitADC();
unsigned char ReadADCLevel();

#endif