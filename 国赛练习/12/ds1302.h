#ifndef __DS1302_H
#define __DS1302_H

#include "main.h"

sbit SCK = P1^7;		
sbit SDA = P2^3;		
sbit RST = P1^3; 

extern TIME read_time;

void DS1302_Init();
void DS1302_Read();

#endif
