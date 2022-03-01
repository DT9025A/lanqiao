#include <STC15.h>
#include <intrins.h>
#include <string.h>

#include "ds1302.h"
#include "ds18b20.h"
#include "display.h"

typedef unsigned char u8;

bit UpdateUI = 1, ShineAlarm = 0;

#define BCD2DIG(x) ((x >> 4) * 10 + (x & 0x0f))
#define DIG2BCD(x) (((x / 10) << 4) + (x % 10))

void Delay5ms() {	//@12.000MHz
    unsigned char i, j;

    i = 59;
    j = 90;
    do {
        while (--j);
    } while (--i);
}

void Periph_Init() {
    Select_Out (5, 0);
    Select_Out (4, 0xff);
    CLEAR_BUFFER;
    P3M0 = P3M1 = 0;
    P3 = 0x0f;
}

void Timer0Init (void) {	//50毫秒@12.000MHz
    AUXR &= 0x7F;	//定时器时钟12T模式
    TMOD &= 0xF0;	//设置定时器模式
    TL0 = 0xB0;		//设置定时初值
    TH0 = 0x3C;		//设置定时初值
    TF0 = 0;		//清除TF0标志
    TR0 = 1;		//定时器0开始计时
    ET0 = 1;
}

void Timer2Init (void) {	//2毫秒@12.000MHz
    AUXR |= 0x04;	//定时器时钟1T模式
    T2L = 0x40;		//设置定时初值
    T2H = 0xA2;		//设置定时初值
    AUXR |= 0x10;	//定时器2开始计时
    IE2 |= 0x04;    //然后再开中断即可
}

void Timer0_ISR() interrupt 1 {
    // 50ms
    static u8 cnt = 0;
    static unsigned int al_cnt = 0;
    if (++cnt == 20) {
        UpdateUI = 1;
        cnt = 0;
    }
    if (ShineAlarm) {
        if (al_cnt % 8 == 0) {
            Select_Out (4, 0xfe);
        } else if (al_cnt % 4 == 0) {
            Select_Out (4, 0xff);
        }
        if (++al_cnt == 100) {
            ShineAlarm = 0;
            al_cnt = 0;
            Select_Out (4, 0xff);
        }
    }
}

void Timer2_ISR() interrupt 12 {
    Display_Refresh();
}

u8 Key_Scan() {
    if (P30 == 0) {
        Delay5ms();
        if (P30 == 0) {
            // while (P30 == 0);
            return 7;
        }
    }
    if (P31 == 0) {
        Delay5ms();
        if (P31 == 0) {
            // while (P31 == 0);
            return 6;
        }
    }
    if (P32 == 0) {
        Delay5ms();
        if (P32 == 0) {
            return 5;
        }
    }
    if (P33 == 0) {
        Delay5ms();
        if (P33 == 0) {
            return 4;
        }
    }
    return 0;
}

u8 Key_Scan_Wait() {
    if (P30 == 0) {
        Delay5ms();
        if (P30 == 0) {
            while (P30 == 0);
            return 7;
        }
    }
    if (P31 == 0) {
        Delay5ms();
        if (P31 == 0) {
            while (P31 == 0);
            return 6;
        }
    }
    if (P32 == 0) {
        Delay5ms();
        if (P32 == 0) {
            while (P32 == 0);
            return 5;
        }
    }
    if (P33 == 0) {
        Delay5ms();
        if (P33 == 0) {
            while (P33 == 0);
            return 4;
        }
    }
    return 0;
}

void SetHMS (u8 *h, u8 *m, u8 *s) {
    u8 flashing;
    flashing = 1;
    Display_Buffer[0] = *h / 10;
    Display_Buffer[1] = *h % 10;
    Display_Buffer[3] = *m / 10;
    Display_Buffer[4] = *m % 10;
    Display_Buffer[6] = *s / 10;
    Display_Buffer[7] = *s % 10;
    do {
        switch (Key_Scan_Wait()) {
            case 4:
                if (flashing == 1) {
                    if (*h > 0)
                        *h -= 1;
                } else if (flashing == 2) {
                    if (*m > 0)
                        *m -= 1;
                } else {
                    if (*s > 0)
                        *s -= 1;
                }
                break;
            case 5:
                if (flashing == 1) {
                    if (*h < 23)
                        *h += 1;
                } else if (flashing == 2) {
                    if (*m < 59)
                        *m += 1;
                } else {
                    if (*s < 59)
                        *s += 1;
                }
                break;
            case 7:
            case 6:
                Display_Buffer[0] = *h / 10;
                Display_Buffer[1] = *h % 10;
                Display_Buffer[3] = *m / 10;
                Display_Buffer[4] = *m % 10;
                Display_Buffer[6] = *s / 10;
                Display_Buffer[7] = *s % 10;
                flashing += 1;
                break;
        }
        if (UpdateUI) {
            switch (flashing) {
                case 1:
                    if (Display_Buffer[0] != 10) {
                        Display_Buffer[0] = Display_Buffer[1] = 10;
                    } else {
                        Display_Buffer[0] = *h / 10;
                        Display_Buffer[1] = *h % 10;
                    }
                    break;

                case 2:
                    if (Display_Buffer[3] != 10) {
                        Display_Buffer[3] = Display_Buffer[4] = 10;
                    } else {
                        Display_Buffer[3] = *m / 10;
                        Display_Buffer[4] = *m % 10;
                    }
                    break;

                case 3:
                    if (Display_Buffer[6] != 10) {
                        Display_Buffer[6] = Display_Buffer[7] = 10;
                    } else {
                        Display_Buffer[6] = *s / 10;
                        Display_Buffer[7] = *s % 10;
                    }
                    break;
            }
            UpdateUI = 0;
        }
    } while (flashing != 4);
}

void main() {
    u8 tmp;
    u8 alarm_h = 0, alarm_m = 0, alarm_s = 0, h, m, s;
    Periph_Init();
    Init_Ds1302();
    Timer0Init();
    Timer2Init();
    EA = 1;
    while (1) {
        if (UpdateUI) {
            h = Read_Ds1302_Byte (0x85);
            Display_Buffer[0] = h / 16;
            Display_Buffer[1] = h % 16;
            Display_Buffer[2] = 11;
            m = Read_Ds1302_Byte (0x83);
            Display_Buffer[3] = m / 16;
            Display_Buffer[4] = m % 16;
            Display_Buffer[5] = 11;
            s = Read_Ds1302_Byte (0x81);
            Display_Buffer[6] = s / 16;
            Display_Buffer[7] = s % 16;
            UpdateUI = 0;
        }
        tmp = Key_Scan();
        switch (tmp) {
            case 4:
                ET0 = 0;
                ShineAlarm = 0;
                Select_Out (4, 0xff);
                CLEAR_BUFFER;
                while (P33 == 0) {
                    tmp = ReadTemp();
                    Display_Buffer[7] = 12;
                    if (tmp != 85) {
                        Display_Buffer[5] = tmp / 10;
                        Display_Buffer[6] = tmp % 10;
                    }
                }
                ET0 = 1;
                UpdateUI = 1;
                break;

            case 7:
                // 时钟设置
                ShineAlarm = 0;
                Select_Out (4, 0xff);
                h = Read_Ds1302_Byte (0x85);
                h = BCD2DIG (h);
                m = Read_Ds1302_Byte (0x83);
                m = BCD2DIG (m);
                s = Read_Ds1302_Byte (0x81);
                s = BCD2DIG (s);
                while (P30 == 0);
                SetHMS (&h, &m, &s);
                Write_Ds1302_Byte (0x8e, 0x00);
                Write_Ds1302_Byte (0x80, DIG2BCD (s));
                Write_Ds1302_Byte (0x82, DIG2BCD (m));
                Write_Ds1302_Byte (0x84, DIG2BCD (h));
                Write_Ds1302_Byte (0x8e, 0x80);
                break;

            case 6:
                // 闹钟设置
                ShineAlarm = 0;
                Select_Out (4, 0xff);
                alarm_h = BCD2DIG (alarm_h);
                alarm_m = BCD2DIG (alarm_m);
                alarm_s = BCD2DIG (alarm_s);
                while (P31 == 0);
                SetHMS (&alarm_h, &alarm_m, &alarm_s);
                alarm_s = DIG2BCD (alarm_s);
                alarm_m = DIG2BCD (alarm_m);
                alarm_h = DIG2BCD (alarm_h);
                break;

            case 5:
                ShineAlarm = 0;
                Select_Out (4, 0xff);
                break;
        }
        if (alarm_h == h && alarm_m == m && alarm_s == s)
            ShineAlarm = 1;
    }
}
