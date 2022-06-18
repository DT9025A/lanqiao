#include "freq.h"

u16 Freq_Result = 0;

void Freq_Init(void)		//2毫秒@12.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
    TMOD |= 0x04;
	TL0 = 0x0;		//设置定时初始值
	TH0 = 0x0;		//设置定时初始值
	TF0 = 0;		//清除TF0标志
}

void Freq_Callback_1s() {
    TR0 = 0;
    Freq_Result = TH0 * 256 + TL0;
    TL0 = 0x0;		//设置定时初始值
	TH0 = 0x0;		//设置定时初始值
	TF0 = 0;		//清除TF0标志
    TR0 = 1;
}
