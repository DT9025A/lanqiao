#include "display.h"

#define DP(x) (x & (~0x80))

unsigned char Disp_Tab[0x20] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0xFF, 0xBF, 0x8E, 0xC1, 0xC8};
unsigned char Display_Buffer[8];

void Timer2Init (void) {	//2ms@12.000MHz
    AUXR |= 0x04;		//Timer clock is 1T mode
    T2L = 0x40;		//Initial timer value
    T2H = 0xA2;		//Initial timer value
    AUXR |= 0x10;		//Timer2 start run
    IE2 |= 0x04;
}

void Tim2_ISR() interrupt 12 {
    static unsigned char i = 0;
    Select_Out (6, 1 << i);
    Select_Out (7, Disp_Tab[Display_Buffer[i]]);
    if (++i == 8) {
        i = 0;
    }
}

void Display_Init() {
    unsigned char i = 0;
    for (i = 0; i < 10; i++)
        Disp_Tab[0x10 + i] = DP (Disp_Tab[i]);
    Display_Clear();
    Timer2Init();
}

void Display_Clear() {
    memset (Display_Buffer, 10, sizeof (Display_Buffer));
}