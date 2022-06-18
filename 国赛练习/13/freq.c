#include "freq.h"

u16 Freq_Result = 0;

void Freq_Init(void)		//2����@12.000MHz
{
	AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
    TMOD |= 0x04;
	TL0 = 0x0;		//���ö�ʱ��ʼֵ
	TH0 = 0x0;		//���ö�ʱ��ʼֵ
	TF0 = 0;		//���TF0��־
}

void Freq_Callback_1s() {
    TR0 = 0;
    Freq_Result = TH0 * 256 + TL0;
    TL0 = 0x0;		//���ö�ʱ��ʼֵ
	TH0 = 0x0;		//���ö�ʱ��ʼֵ
	TF0 = 0;		//���TF0��־
    TR0 = 1;
}
