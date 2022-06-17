#include "sonic.h"

void Sonic_Init() {
    CMOD |= 0x08; //����ʱ��ʱ������ΪSYSSclk������Ϊ11.0592MHZ��,��Ϊ���ò�ѯ�����ر�����ж�
    CCON = 0x00;
}

void Delay13us() {	//@12.000MHz
    unsigned char i;

    _nop_();
    _nop_();
    i = 36;
    while (--i);
}

void Sonic_SendTrig() {
    u8 i;
    for (i = 0; i < 8; i++) {
        TX = 1;
        Delay13us();
        TX = 0;
        Delay13us();
    }
}

u16 Sonic_TrigMeasure() {
    u16 time, cnt;
    Sonic_SendTrig();
    CH = 0;
    CL = 0;
    CF = 0;
    CR = 1;
    cnt = 0;
    while (RX)
        if (CF == 1) {
            cnt += 1;
            CF = 0;
        }
    time = CH * 256 + CL;
    return (u16) ((time * 0.017) / 12.0 + 92.841 * cnt);

}
