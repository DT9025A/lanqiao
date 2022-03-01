#include "display.h"

//                                                                                    10
unsigned char code Display_Tab[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0xff, 0xbf, 0xc6};
unsigned char Display_Buffer[8];

void Select_Out (unsigned char x, unsigned char dat) {
    P2 &= 0x1f;
    P2 |= x << 5;
    P0 = dat;
    _nop_();
    P2 &= 0x1f;
    P0 = 0;
}

void Display_Refresh() {
    static unsigned char dig = 0;

    Select_Out (6, 1 << dig);
    Select_Out (7, Display_Tab[Display_Buffer[dig ++]]);

    if (dig == 8)
        dig = 0;
}
