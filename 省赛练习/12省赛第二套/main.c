// 13:30-15:14
#include "main.h"

bit Count_Fin = 0, Update = 0, VoltageSave = 0, FreqSave = 0, LED_On = 1;
unsigned char Display_Menu = MENU_F, Systick = 0, Analog_Channel = 3, LED = 0XFF, Last_Led = 0;
unsigned int Freq = 0, SavedFreq = 65535;
float Voltage = 0.0, CH3Voltage = 0.0, SavedCH3Voltage = 5.0;

void Select_Out (unsigned char x, unsigned char dat) {
    P2 &= 0x1F;
    P2 |= (x << 5);
    P0 = dat;
    _nop_();
    P2 &= 0x1F;
    P0 = 0;
}

void Periph_Init() {
    P3M0 = P3M1 = 0;
    Select_Out (4, 0xff);
    Select_Out (5, 0);
    Display_Init();
    EA = 1;
}

void Timer0Init (void) {
    AUXR &= 0x7F;
    TMOD &= 0xF0;
    TMOD |= 0x04;   //T0 Counter
    TL0 = 0;
    TH0 = 0;
    TF0 = 0;
}

void Timer1Init (void) {	//50ms@12.000MHz
    AUXR &= 0xBF;		//Timer clock is 12T mode
    TMOD &= 0x0F;		//Set timer work mode
    TL1 = 0xB0;		//Initial timer value
    TH1 = 0x3C;		//Initial timer value
    TF1 = 0;		//Clear TF1 flag
    TR1 = 1;		//Timer1 start run
    ET1 = 1;
}

void T1_ISR() interrupt 3 {
    if (++Systick == 20) {
        TR0 = TR1 = 0;
        Count_Fin = 1;
        Systick = 0;
    }

    if(Systick % 10 == 0)
        Update = 1;
}

void Refresh_Data() {
    unsigned int tmp;
    Update = 0;
    switch (Display_Menu) {
        case MENU_F:
            Display_Clear();
            Display_Buffer[0] = CHAR_F;
            tmp = Freq;
            if (Freq > 9999) {
                Display_Buffer[3] = tmp / 10000;
                tmp %= 10000;
            }
            if (Freq > 999) {
                Display_Buffer[4] = tmp / 1000;
                tmp %= 1000;
            }
            if (Freq > 99) {
                Display_Buffer[5] = tmp / 100;
                tmp %= 100;
            }
            if (Freq > 9) {
                Display_Buffer[6] = tmp / 10;
                tmp %= 10;
            }
            Display_Buffer[7] = tmp;
            break;

        case MENU_N:
            Display_Clear();
            Display_Buffer[0] = CHAR_N;
            tmp = 1000000 / Freq;
            if (tmp > 9999) {
                Display_Buffer[3] = tmp / 10000;
                tmp %= 10000;
            }
            if (tmp > 999) {
                Display_Buffer[4] = tmp / 1000;
                tmp %= 1000;
            }
            if (tmp > 99) {
                Display_Buffer[5] = tmp / 100;
                tmp %= 100;
            }
            if (tmp > 9) {
                Display_Buffer[6] = tmp / 10;
                tmp %= 10;
            }
            Display_Buffer[7] = tmp;
            break;

        case MENU_U:
            Display_Clear();
            Display_Buffer[0] = CHAR_U;
            Display_Buffer[1] = CHAR_DEC;
            Display_Buffer[2] = Analog_Channel;

            tmp = Voltage * 100;
            Display_Buffer[5] = (tmp / 100) + 0x10;
            Display_Buffer[6] = (tmp % 100) / 10;
            Display_Buffer[7] = tmp % 10;
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

unsigned char Key_Scan() {
    unsigned char len;
    P3 |= 0x0F;
    if (P30 == 0) {
        Delay20ms();
        if (P30 == 0) { 
            len = 0;
            while (P30 == 0 && len < 100) {
                Delay20ms();
                len += 1;
            }
            if (len >= 49)
                return 8;
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
    Periph_Init();
    Timer0Init();
    Timer1Init();

    while (1) {
        switch (Key_Scan()) {
            case 4:
                Display_Menu += 1;
                if (Display_Menu == MENU_U) {
                    Analog_Channel = 1;
                    PCF8591_Init (Analog_Channel);
                }
                if (Display_Menu == MENU_U + 1) {
                    Display_Menu = MENU_F;
                }
                break;

            case 5:
                if (Display_Menu == MENU_U) {
                    Analog_Channel = (Analog_Channel == 1 ? 3 : 1);
                }
                break;

            case 6:
                VoltageSave = 1;
                break;

            case 7:
                FreqSave = 1;
                break;

            case 8:
                LED_On = !LED_On;
                break;
        }

        if (Count_Fin) {
            Freq = TH0 * 256 + TL0;
            Count_Fin = 0;
            TL0 = TH0 = 0;
            TL1 = 0xB0;		//Initial timer value
            TH1 = 0x3C;		//Initial timer value
            TR0 = TR1 = 1;
        }

        PCF8591_Init (3);
        CH3Voltage = (float)PCF8591_Read() * 0.0196;

        PCF8591_Init (Analog_Channel);
        Voltage = (float)PCF8591_Read() * 0.0196;

        if (VoltageSave) {
            VoltageSave = 0;
            SavedCH3Voltage = CH3Voltage;
        }

        if (FreqSave) {
            FreqSave = 0;
            SavedFreq = Freq;
        }

        if (LED_On) {
            LED = 0;
            if (CH3Voltage > SavedCH3Voltage) {
                LED |= 0x01;
            }
            if (Freq > SavedFreq) {
                LED |= 0x02;
            }
            LED |= 1 << (Display_Menu + 2);
        } else {
            LED = 0;
        }

        if (LED != Last_Led) {
            Last_Led = LED;
            Select_Out (4, ~LED);
        }

        if (Update)
            Refresh_Data();

    }
}