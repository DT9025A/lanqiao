#ifndef __DISPLAY_H
#define __DISPLAY_H

#include "main.h"

extern u8 DispBuf[8];

void Display_Init();
void Display_Clear();

enum {
    CHAR_DEC = 10,
    CHAR_L,
    CHAR_C,
    CHAR_H,
    CHAR_P,
    CHAR_F,
    CHAR_TYPE_MAX = 0x1a,
    CHAR_TYPE_AVG,
    CHAR_TYPE_MIN,
    CHAR_NUL = 0x1f
};

#endif
