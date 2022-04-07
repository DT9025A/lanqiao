#include "display.h"

u8 code Tab[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0xff, 0xC6, 0x8C};
u8 Display_Buffer[8];

void Timer2Init (void) {	//2ms@12.000MHz
    AUXR |= 0x04;		//Timer clock is 1T mode
    T2L = 0x40;		//Initial timer value
    T2H = 0xA2;		//Initial timer value
    AUXR |= 0x10;		//Timer2 start run
    IE2 |= 0x04;
}

void T2_ISR() interrupt 12 {
    static u8 i = 0;
    Select_Out (6, 1 << i);
    Select_Out (7, Tab[Display_Buffer[i]]);
    if (++i == 8) {
        i = 0;
    }
}

void Display_Init() {
    Display_Clear();
    Timer2Init();
}

void Display_Clear() {
    memset (Display_Buffer, 10, sizeof (Display_Buffer));
}
