#ifndef __ONEWIRE_H
#define __ONEWIRE_H

#include "main.h"

sbit DQ = P1^4;  

u8 rd_temperature(void);  

#endif
