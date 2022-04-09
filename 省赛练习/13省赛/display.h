#ifndef __DISPLAY_H_
#define __DISPLAY_H_

#include "main.h"

// 显示缓冲区
extern u8 Display_Buffer[8];

// 两个特殊字符
enum {
    CHAR_U = 11,
    CHAR_DEC
};

void Display_Clear();
void Display_Init();

#endif
