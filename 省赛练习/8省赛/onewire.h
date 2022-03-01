#ifndef __ONEWIRE_H
#define __ONEWIRE_H

#include <STC15.h>
#include <intrins.h>

void Write_DS18B20 (unsigned char dat);
unsigned char Read_DS18B20 (void);
bit init_ds18b20 (void);

#endif
