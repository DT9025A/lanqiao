#include "display.h"

// 字模
u8 Tab[0x20] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0xff, 0xc1, 0xbf};
// 显示缓冲区
u8 Display_Buffer[8];

void Display_Clear() {
    // 清除缓冲区，全填充不亮的字模下标
    memset (Display_Buffer, 10, sizeof (Display_Buffer));
}

void Timer2Init (void) {	//2毫秒@12.000MHz
    AUXR |= 0x04;		//定时器时钟1T模式
    T2L = 0x40;		//设置定时初值
    T2H = 0xA2;		//设置定时初值
    AUXR |= 0x10;		//定时器2开始计时
    IE2 |= 0x04;
}

void Display_Init() {
    u8 i;
    // 构造带有小数点的字模
    for (i = 0; i < 10; i++)
        Tab[0x10 + i] = Tab[i] & ~0x80;
    // 清除缓冲区
    Display_Clear();
    // 初始化定时器2
    Timer2Init();
}

// 定时器2中断处理函数，在这里对数码管进行显示扫描
void T2_ISR() interrupt 12 {
    static u8 i = 0;

    // 选择单个数码管
    Select_Out (6, 1 << i);
    // 送显示数据
    Select_Out (7, Tab[Display_Buffer[i]]);

    // 限制下标边界
    if (++i == 8)
        i = 0;
}
