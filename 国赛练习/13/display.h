#ifndef __DISPLAY_H_
#define __DISPLAY_H_

#include "main.h"
#include "string.h"

extern u8 DispBuf[8];

enum {
    CHAR_NUL = 10,
    CHAR_A,
    CHAR_H,
    CHAR_P,
    CHAR_F
};

void Display_Init();
void Display_Refresh();
void Display_Clear();

#endif
