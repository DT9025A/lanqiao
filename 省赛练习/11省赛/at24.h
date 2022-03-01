#ifndef __AT24_H_
#define __AT24_H_

#include "main.h"
#include "iic.h"

void AT24C02_WriteByte(u8 addr, u8 dat);
u8 AT24C02_ReadByte(u8 addr);

#endif