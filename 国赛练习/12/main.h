#ifndef __MAIN_H_
#define __MAIN_H_

#include <STC15F2K60S2.H>
#include <string.h>
#include <intrins.h>

#define LIGHT_THRESHOLD 128

typedef unsigned char u8;
typedef unsigned int u16;
typedef struct {
    u8 hh;
    u8 mm;
    u8 ss;
}TIME;

enum {
    MENU_TIME,
    MENU_DISTANCE,
    MENU_RECORD,
    MENU_PARAMETER
};

enum {
    //¶¨Ê±
    DISTANCE_MODE_F = 0,
    //´¥·¢
    DISTANCE_MODE_C = 1
};

void Select_Out(u8 x, u8 d);

#endif
