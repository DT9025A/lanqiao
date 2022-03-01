#ifndef __DS18B20_H_
#define __DS18B20_H_

#include <STC15.H>
#include <intrins.h>

bit Init_DS18B20 (void);
unsigned char Read_DS18B20 (void);
void Write_DS18B20 (unsigned char dat);
float DS18B20_ReadTemp();

#endif
