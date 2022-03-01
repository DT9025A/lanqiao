#ifndef __DISPLAY_H_
#define __DISPLAY_H_

#include <STC15.h>
#include <intrins.h>

extern unsigned char Display_Buffer[8];

#define CLEAR_BUFFER memset(Display_Buffer, 10, sizeof(Display_Buffer))
void Display_Refresh();
void Select_Out (unsigned char x, unsigned char dat);

#endif
