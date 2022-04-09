#ifndef __MAIN_H_
#define __MAIN_H_

#include <STC15.H>
#include <intrins.h>
#include <string.h>

typedef unsigned char u8;
typedef unsigned int u16;

// 显示菜单枚举值
enum {
    MENU_TEMP = 1,
    MENU_TIME,
    MENU_ARG
};

// 工作模式枚举值
enum {
    MODE_TEMP = 0,
    MODE_TIME = 1
};

// 按键状态结构体
typedef struct {
    u8 S17;
    u8 S16;
    u8 S12;
    u8 S13;
} KeyStatusGroup;

// 外设控制结构体
typedef struct {
    // 当前状态
    u8 Status;
    // 上一个状态
    u8 LastStatus;
} PeriphControlGroup;

void Select_Out (u8 x, u8 d);
void Key_Scan();
void Timer0Init (void);
void Periph_Init();

#endif
