#include "display.h"

u8 DispBuf[8];

u8 Seg_Table[0x20] = {
    0xc0, //0
    0xf9, //1
    0xa4, //2
    0xb0, //3
    0x99, //4
    0x92, //5
    0x82, //6
    0xf8, //7
    0x80, //8
    0x90, //9
    0xbf, //-
    0xc7, //L
    0xc6, //C
    0x89, //H
    0x8c, //P
    0x8e //F
};

void Timer2Init (void) {	//2ms@12.000MHz
    AUXR |= 0x04;		//Timer clock is 1T mode
    T2L = 0x40;		//Initial timer value
    T2H = 0xA2;		//Initial timer value
    AUXR |= 0x10;		//Timer2 start run
    IE2 |= 0x04;
}

void T2INT() interrupt 12 {
    static u8 i = 0;

    Select_Out (6, 1 << i);
    Select_Out (7, Seg_Table[DispBuf[i]]);

    if (++i == 8)
        i = 0;
}

void Display_Clear() {
    memset (DispBuf, 0x1f, sizeof (DispBuf));
}

void Display_Init() {
    u8 i;

    Seg_Table[0x1a] = 0xfe;
    Seg_Table[0x1b] = 0xbf;
    Seg_Table[0x1c] = 0xf7;
    Seg_Table[0x1f] = 0xff;
    Display_Clear();
    Timer2Init();
    for (i = 0; i < 10; i++)
        Seg_Table[0x10 + i] = Seg_Table[i] & (~0x80);
}
