#ifndef __DISPLAY_H_
#define __DISPLAY_H_

#include <STC15.H>
#include <intrins.h>
#include <string.h>

extern unsigned char Display_Buffer[8];
                          
void Select_Out(unsigned char x, unsigned char dat);

void Display_Init();
void Display_Clear();

#endif
