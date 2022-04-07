// 12:10 - 1:36
#include "main.h"
#include "display.h"
#include "onewire.h"
#include "pcf.h"

bit Menu = MENU_C;
char Tmax = 30, Tmin = 20, T;

void Select_Out (u8 x, u8 dat) {
    P2 &= 0x1F;
    P0 = dat;
    P2 |= (x << 5);
    _nop_();
    P2 &= 0x1F;
    P0 = 0;
}

void Periph_Init() {
    rd_temperature();
    P3M0 = P3M1 = 0;
    Display_Init();
    Select_Out (4, 0xff);
    Select_Out (5, 0);
    EA = 1;
}

void Delay1000ms() {	//@12.000MHz
    unsigned char i, j, k;

    _nop_();
    _nop_();
    i = 46;
    j = 153;
    k = 245;
    do {
        do {
            while (--k);
        } while (--j);
    } while (--i);
}

void Refresh_Display (u8 tTmax, u8 tTmin) {
    switch ((u8)Menu) {
        case MENU_C:
            Display_Buffer[0] = CHAR_C;
            if (T >= 85)
                T = 0;
            Display_Buffer[6] = T / 10;
            Display_Buffer[7] = T % 10;
            break;

        case MENU_P:
            Display_Buffer[0] = CHAR_P;

            Display_Buffer[3] = tTmax / 10;
            Display_Buffer[4] = tTmax % 10;

            Display_Buffer[6] = tTmin / 10;
            Display_Buffer[7] = tTmin % 10;
            break;
    }
}

void Delay20ms() {	//@12.000MHz
    unsigned char i, j, k;

    _nop_();
    _nop_();
    i = 1;
    j = 234;
    k = 113;
    do {
        do {
            while (--k);
        } while (--j);
    } while (--i);
}

u8 Key_Scan() {
    P3 |= 0x0F;
    if (P30 == 0) {
        Delay20ms();
        if (P30 == 0) {
            while (P30 == 0);
            return 7;
        }
    }
    if (P31 == 0) {
        Delay20ms();
        if (P31 == 0) {
            while (P31 == 0);
            return 6;
        }
    }
    if (P32 == 0) {
        Delay20ms();
        if (P32 == 0) {
            while (P32 == 0);
            return 5;
        }
    }
    if (P33 == 0) {
        Delay20ms();
        if (P33 == 0) {
            while (P33 == 0);
            return 4;
        }
    }
    return 0;
}

void main() {
    u8 tTmax, tTmin, *currentT, LED = 0, LastLED = 0xff;
    Periph_Init();

    while (1) {
        T = rd_temperature();
        if (T < 85) {
            LED &= 0xF8;
            if (T > Tmax) {
                PCF_DAC_Out (204);
                LED |= 0x01;
            } else if (T < Tmin) {
                PCF_DAC_Out (102);
                LED |= 0x04;
            } else {
                PCF_DAC_Out (153);
                LED |= 0x02;
            }

            switch (Key_Scan()) {
                case 4:
                    Menu = ! Menu;
                    Display_Clear();
                    if (Menu == CHAR_P) {
                        tTmax = Tmax;
                        tTmin = Tmin;
                        currentT = &tTmin;
                    } else {
                        if (tTmax >= tTmin) {
                            Tmax = tTmax;
                            Tmin = tTmin;
                            LED &= 0xF7;
                        } else {
                            LED |= 0x08;
                        }
                    }
                    break;

                case 5:
                    if (currentT == &tTmin) {
                        currentT = &tTmax;
                    } else {
                        currentT = &tTmin;
                    }
                    break;

                case 6:
                    if (*currentT < 99)
                        *currentT += 1;
                    break;

                case 7:
                    if (*currentT > 0)
                        *currentT -= 1;
                    break;
            }
        }
        Refresh_Display (tTmax, tTmin);
        if (LED != LastLED) {
            LastLED = LED;
            Select_Out (4, ~LED);
        }
        Delay20ms();
    }
}
