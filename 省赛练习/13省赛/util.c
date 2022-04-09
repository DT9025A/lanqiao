#include "main.h"
#include "display.h"
#include "onewire.h"
#include "ds1302.h"

// 在main.c的变量
extern KeyStatusGroup KeyStatus, LastKeyStatus;

// 向指定位置输出数据
void Select_Out (u8 x, u8 d) {
    P2 &= 0x1F;
    P0 = d;
    P2 |= (x << 5);
    _nop_();
    P2 &= 0x1F;
}

void Delay100ms() {	//@12.000MHz
    unsigned char i, j, k;

    _nop_();
    _nop_();
    i = 5;
    j = 144;
    k = 71;
    do {
        do {
            while (--k);
        } while (--j);
    } while (--i);
}

// 初始化外设
void Periph_Init() {
    // 关灯
    Select_Out (4, 0xff);
    // 关外设
    Select_Out (5, 0);
    // 初始化数码管显示
    Display_Init();
    // 初始化DS1302
    DS1302_Init();
    // 提前触发一次温度转换，但好像没啥用
    rd_temperature();
    // 等待转换
    Delay100ms();
    // 初始化T0
    Timer0Init();
    // 开总中断
    EA = 1;
}

void Delay10ms() {	//@12.000MHz
    unsigned char i, j;

    i = 117;
    j = 184;
    do {
        while (--j);
    } while (--i);
}

void Timer0Init (void) {	//25毫秒@12.000MHz
    AUXR &= 0x7F;		//定时器时钟12T模式
    TMOD &= 0xF0;		//设置定时器模式
    TL0 = 0x58;		//设置定时初值
    TH0 = 0x9E;		//设置定时初值
    TF0 = 0;		//清除TF0标志
    TR0 = 1;		//定时器0开始计时
    ET0 = 1;
}

void Key_Scan() {
    // 初始化P3状态为全1
    P3 = 0xFF;

    //保存上一次状态，清零当前状态
    memcpy (&LastKeyStatus, &KeyStatus, sizeof (KeyStatusGroup));
    memset (&KeyStatus, 0, sizeof (KeyStatusGroup));

    // 下面就是基础的矩阵扫描
    P34 = 0;
    if (P32 == 0) {
        Delay10ms();
        if (P32 == 0) {
            KeyStatus.S17 = 1;
            return;
        }
    }
    if (P33 == 0) {
        Delay10ms();
        if (P33 == 0) {
            KeyStatus.S16 = 1;
            return;
        }
    }
    P34 = 1;
    P35 = 0;
    if (P32 == 0) {
        Delay10ms();
        if (P32 == 0) {
            KeyStatus.S13 = 1;
            return;
        }
    }
    if (P33 == 0) {
        Delay10ms();
        if (P33 == 0) {
            KeyStatus.S12 = 1;
            return;
        }
    }
}
