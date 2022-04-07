#ifndef __MAIN_H_
#define __MAIN_H_

#include <STC15.H>
#include <intrins.h>

typedef unsigned char u8;
typedef unsigned int u16;

void Select_Out(u8 x, u8 dat);

enum {
    MENU_C = 0,
    MENU_P
};

#endif
