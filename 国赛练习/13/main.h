#ifndef __MAIN_H_
#define __MAIN_H_

#include <STC15F2K60S2.H>
#include <INTRINS.H>

typedef unsigned char u8;
typedef unsigned int u16;

#define RELAY 0x10
#define MOTOR 0x20 

void Select_Out(u8 x, u8 dat);

#endif
