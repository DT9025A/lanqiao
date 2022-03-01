#include "display.h"

//                                                                                  U      P     n
u8 Tab[0x20] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0xff, 0xc1, 0x8c, 0xc8};
u8 Display_Buffer[8];

void Timer2Init (void) {	//2毫秒@12.000MHz
    AUXR |= 0x04;		//定时器时钟1T模式
    T2L = 0x40;		//设置定时初值
    T2H = 0xA2;		//设置定时初值
    AUXR |= 0x10;		//定时器2开始计时
    IE2 |= 0x04;
}

void TIM2_ISR() interrupt 12 {
    static u8 i = 0;
    Select_Out (6, 1 << i);
    Select_Out (7, Tab[Display_Buffer[i]]);

    if (++i == 8)
        i = 0;
}

void Display_Init() {
    u8 i;
    Display_Clear();
    for (i = 0; i < 10; i++)
        Tab[i + 0x10] = Tab[i] & (~0x80);
    Timer2Init();
}

void Display_Clear() {
    memset (Display_Buffer, 10, sizeof (Display_Buffer));
}
