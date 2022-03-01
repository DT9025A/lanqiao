#ifndef __MAIN_H_
#define __MAIN_H_

#include <STC15.H>
#include <intrins.h>
#include <string.h>

typedef unsigned char u8;
typedef unsigned int u16;

enum {
    M_U,
    M_P,
    M_N
};

void Select_Out(u8 x, u8 dat);

#endif
