#ifndef __DISPLAY_H_
#define __DISPLAY_H_

#include "main.h"
#include <string.h>

extern u8 Display_Buffer[8];

enum {
    CHAR_C = 11,
    CHAR_P
};

void Display_Init();
void Display_Clear();

#endif