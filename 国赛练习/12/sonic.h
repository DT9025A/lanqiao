#ifndef __SONIC_H_
#define __SONIC_H_

#include "main.h"

sbit RX = P1 ^ 1;
sbit TX = P1 ^ 0;

void Sonic_Init();
u16 Sonic_TrigMeasure();

#endif
