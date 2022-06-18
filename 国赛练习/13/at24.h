#ifndef __AT24_H_
#define __AT24_H_

#include "main.h"
#include "iic.h"

void AT24_Write(u8 addr, u8 dat);
u8 AT24_Read(u8 addr);

#endif
