/*
    T0: 时基信号 50MS
    T1: 显示刷新 40MS
    T2: 显示刷新信号 2MS
*/

#include <STC15.H>
#include <intrins.h>
#include <string.h>

#include "at24c02.h"
#include "adc.h"

// 数码管
unsigned char code Disp_Tab[] = {0xc0, 0xf9, 0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0xff, 0xbf};
unsigned char Disp[8];

// LED
unsigned char code LED_Tab_M3[] = {0x7e, 0xbd, 0xdb, 0xe7};
unsigned char LED_Disp = 0xfe, LED_Shine_Target = 8, LED_Disp_Mode = 1;
unsigned char LED_Disp_Change_Tick[4] = {8, 8, 8, 8};

// KEY
bit s7 = 0, s6 = 0, s5 = 0, s4 = 0;

// BLINK
bit Blink_Target = 0;

// TICK
unsigned char Systick_50ms = 0;

void Select_Out (unsigned char x, unsigned char dat) {
    P2 &= 0x1F;
    P2 |= (x & 0x07) << 5;
    P0 = dat;
    _nop_();
    P2 &= 0x1F;
    P0 = 0;
}

void Periph_Init() {
    Select_Out (5, 0x00);
    Select_Out (4, 0xff);
    memset (Disp, 10, sizeof (Disp));
    P3M1 = P3M0 = 0;
}

void Timer2Init (void) {	//2毫秒@12.000MHz
    AUXR |= 0x04;		//定时器时钟1T模式
    T2L = 0x40;		//设置定时初值
    T2H = 0xA2;		//设置定时初值
    AUXR |= 0x10;		//定时器2开始计时
    IE2 |= 0x04;                    //然后再开中断即可
}

void Timer1Init (void) {	//40毫秒@12.000MHz
    AUXR &= 0xBF;		//定时器时钟12T模式
    TMOD &= 0x0F;		//设置定时器模式
    TL1 = 0xC0;		//设置定时初值
    TH1 = 0x63;		//设置定时初值
    TF1 = 0;		//清除TF1标志
    ET1 = 1;
}

void Timer0Init (void) {	//50毫秒@12.000MHz
    AUXR &= 0x7F;		//定时器时钟12T模式
    TMOD &= 0xF0;		//设置定时器模式
    TL0 = 0xB0;		//设置定时初值
    TH0 = 0x3C;		//设置定时初值
    TF0 = 0;		//清除TF0标志
    TR0 = 1;		//定时器0开始计时
    ET0 = 1;
}

void Timer2_ISR() interrupt 12 {
    static char d = 0;
    static int led_cnt = 0;

    Select_Out (6, 1 << d);
    Select_Out (7, Disp_Tab[Disp[d]]);

    if (led_cnt <= LED_Shine_Target)
        Select_Out (4, LED_Disp);
    else
        Select_Out (4, 0xff);

    if (d == 8)
        d = 0;
    else
        d += 1;

    if (led_cnt == 8)
        led_cnt = 0;
    else
        led_cnt++;
}

void Timer1_ISR() interrupt 3 {
    static unsigned char cnt = 0, tmp;
    static bit lighting = 1;

    if (cnt == 19) {
        cnt = 0;
        if (lighting)  {
            if (Blink_Target) {
                //闪烁间隔
                Disp[4] = Disp[5] = Disp[6] = Disp[7] = 10;
                Disp[0] = 11;
                Disp[1] = LED_Disp_Mode;
                Disp[2] = 11;
            } else {
                //闪烁模式
                Disp[0] = Disp[1] = Disp[2] = 10;
                tmp = LED_Disp_Change_Tick[LED_Disp_Mode - 1];
                if (tmp >= 20)
                    Disp[4] = 1;
                else
                    Disp[4] = 10;
                tmp %= 20;
                Disp[5] = tmp / 2;
                Disp[6] = Disp[7] = 0;
            }
            lighting = 0;
        } else {
            if (Blink_Target) {
                //闪烁间隔
                tmp = LED_Disp_Change_Tick[LED_Disp_Mode - 1];
                if (tmp >= 20)
                    Disp[4] = 1;
                else
                    Disp[4] = 10;
                tmp %= 20;
                Disp[5] = tmp / 2;
                Disp[6] = Disp[7] = 0;
            } else {
                //闪烁模式
                Disp[0] = 11;
                Disp[1] = LED_Disp_Mode;
                Disp[2] = 11;
            }
            lighting = 1;
        }
    } else {
        cnt += 1;
    }
}

void Timer0_ISR() interrupt 1 {
    static unsigned char step = 0;
    if (Systick_50ms % LED_Disp_Change_Tick[LED_Disp_Mode - 1] == 0)
        switch (LED_Disp_Mode) {
            case 1:
                LED_Disp = ~ (1 << step++);
                if (step == 8) {
                    LED_Disp_Mode = 2;
                    step = 1;
                    break;
                }
                break;

            case 2:
                LED_Disp = ~ (0x80 >> step++);
                if (step == 8) {
                    LED_Disp_Mode = 3;
                    step = 0;
                    break;
                }
                break;

            case 3:
                if (step == 4) {
                    LED_Disp_Mode = 4;
                    step = 1;
                    break;
                }
                LED_Disp = LED_Tab_M3[step++];
                break;

            case 4:
                if (step == 4) {
                    LED_Disp_Mode = 1;
                    step = 0;
                    break;
                }
                LED_Disp = LED_Tab_M3[3 - (step++)];
                break;
        }
    if (++Systick_50ms == 25)
        Systick_50ms = 1;
}

void Load_Settings() {
    unsigned char i, j;
    for (i = 0; i < 4; i++) {
        j = ReadEEPROM (i);
        LED_Disp_Change_Tick[i] = (j > 24 || j < 4) ? 8 : j;
    }
}

void Key_Scan() {
    if (P30 == 0) {
        s7 = 1;
        while (P30 == 0);
    }
    if (P31 == 0) {
        s6 = 1;
        while (P31 == 0);
    }
}

void Key_Scan_In() {
    if (P31 == 0) {
        s6 = 1;
        while (P31 == 0);
    } else if (P32 == 0) {
        s5 = 1;
        while (P32 == 0);
    } else if (P33 == 0) {
        s4 = 1;
        while (P33 == 0);
    }
}

void main() {
    unsigned char tmp;
    bit P33F = 0;
    Periph_Init();
    Timer0Init();
    Timer1Init();
    Timer2Init();
    Load_Settings();
    InitADC();
    P3 = 0x0f;
    EA = 1;
    while (1) {
        LED_Shine_Target = (ReadADCLevel() / 64 + 1) * 2;
        Key_Scan();
        if (s7) {
            ET0 = !ET0;
            s7 = 0;
        }
        if (s6) {
            memset (Disp, 10, sizeof (Disp));
            Disp[0] = 11;
            Disp[1] = LED_Disp_Mode;
            Disp[2] = 11;
            tmp = LED_Disp_Change_Tick[LED_Disp_Mode - 1];
            if (tmp >= 20)
                Disp[4] = 1;
            else
                Disp[4] = 10;
            tmp %= 20;
            Disp[5] = tmp / 2;
            Disp[6] = Disp[7] = 0;
            TR1 = 1;
            s6 = 0;
            while (1) {
                Key_Scan_In();
                if (s6) {
                    if (Blink_Target == 0)
                        Blink_Target = 1;
                    else {
                        for (tmp = 0; tmp < 4 ; tmp++)
                            WriteEEPROM (tmp, LED_Disp_Change_Tick[tmp]);
                        goto out;
                    }
                    s6 = 0;
                }
                if (s5) {
                    if (Blink_Target == 0)  {
                        LED_Disp_Mode += 1;
                        if (LED_Disp_Mode == 5)
                            LED_Disp_Mode = 1;
                    } else {
                        LED_Disp_Change_Tick[LED_Disp_Mode - 1] += 2;
                        if (LED_Disp_Change_Tick[LED_Disp_Mode - 1] == 26)
                            LED_Disp_Change_Tick[LED_Disp_Mode - 1] = 8;
                    }
                    s5 = 0;
                }
                if (s4) {
                    if (Blink_Target == 0)  {
                        LED_Disp_Mode -= 1;
                        if (LED_Disp_Mode == 0)
                            LED_Disp_Mode = 4;
                    } else {
                        LED_Disp_Change_Tick[LED_Disp_Mode - 1] -= 2;
                        if (LED_Disp_Change_Tick[LED_Disp_Mode - 1] == 6)
                            LED_Disp_Change_Tick[LED_Disp_Mode - 1] = 24;
                    }
                    s4 = 0;
                }
            }
out:
            TR1 = 0; 
            s6 = 0;
            Blink_Target = 0;
            memset (Disp, 10, sizeof (Disp));
        }
        if (P33 == 0) {
            memset (Disp, 10, sizeof (Disp));
            Disp[6] = 11;
            Disp[7] = LED_Shine_Target / 2;
            P33F = 1;
        }
        if (P33 && P33F) {
            memset (Disp, 10, sizeof (Disp));
            P33F = 0;
        }
    }
}
