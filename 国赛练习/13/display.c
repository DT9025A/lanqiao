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
    0xff, //NUL
    0x88, //A
    0x89, //H
    0x8c, //P
    0x8e //F
};

void Display_Refresh() {
    static u8 i = 0;

    Select_Out (6, 1 << i);
    Select_Out (7, Seg_Table[DispBuf[i++]]);

    if(i == 8)
        i = 0;
}

void Display_Init() {
    u8 i;
    for (i = 0; i < 10; i++) {
        Seg_Table[0x10 + i] = Seg_Table[i] & (~0x80);
    }
    Display_Clear();
}

void Display_Clear() {
    memset (DispBuf, CHAR_NUL, sizeof (DispBuf));
}
