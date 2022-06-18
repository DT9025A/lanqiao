#include "sonic.h"

sbit TX = P1 ^ 0;
sbit RX = P1 ^ 1;

void Sonic_Init() {
    CMOD = 0x88;
    CCON = 0;
    CH = 0;
    CL = 0;
}

void Delay13us()		//@12.000MHz
{
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

u16 Sonic_Measure() {
    u8 cf = 0;

    Sonic_Init();  
    Sonic_SendTrig(); 
    CR = 1;
    while(RX) {
        if(CF == 1) {
            cf++;
            CF = 0;
        }
        if(cf == 10)
            return 65535;
    }  
    CR = 0;
    return (u16)(((CH * 256 + CL) * 0.017) / 12 + 92.8427 * cf);
}
