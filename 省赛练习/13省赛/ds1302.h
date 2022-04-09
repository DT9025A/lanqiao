#ifndef __DS1302_H
#define __DS1302_H

#include "main.h"

sbit SCK = P1 ^ 7;
sbit SDA = P2 ^ 3;
sbit RST = P1 ^ 3;

// 时间结构体
typedef struct _TIME {
    u8 hh;
    u8 mm;
    u8 ss;
} TIME;

// 当前时间结构体变量
extern TIME currentTime;

void DS1302_Init();
void DS1302_Read_Current_Time();
void DS1302_Write_Time (TIME *src);

#endif
