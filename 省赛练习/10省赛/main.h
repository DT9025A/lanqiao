#ifndef __MAIN_H_
#define __MAIN_H_

#include <STC15.H>
#include <intrins.h>

typedef unsigned char u8;
typedef unsigned int u16;

enum {
    M_U = 0,
    M_F
};

void Select_Out(u8 x, u8 dat);

#endif
