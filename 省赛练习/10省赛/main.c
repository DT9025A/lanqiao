#include "main.h"
#include "display.h"
#include "pcf.h"

bit UpdateRequire = 1, CountFinish = 0, DACStaticOut = 1, LEDEnable = 1, DisplayEnable = 1;
u8 Menu = M_U, DACOut = 102, LEDOut = 0xff, LastLEDOut = 0, SysTick = 0;
u16 Frequency = 0;

void Select_Out (u8 x, u8 dat) {
    P2 &= 0x1f;
    P2 |= (x << 5);
    P0 = dat;
    _nop_();
    P2 &= 0x1f;
    P0 = dat;
}

void Timer0Init (void) {	//2毫秒@12.000MHz
    AUXR |= 0x80;		//定时器时钟1T模式
    TMOD &= 0xF0;		//设置定时器模式
    TMOD |= 0x04;     //16位计数模式
    TL0 = 0;		//设置定时初值
    TH0 = 0;		//设置定时初值
    TF0 = 0;		//清除TF0标志
}

void Timer1Init (void) {	//50毫秒@12.000MHz
    AUXR &= 0xBF;		//定时器时钟12T模式
    TMOD &= 0x0F;		//设置定时器模式
    TL1 = 0xB0;		//设置定时初值
    TH1 = 0x3C;		//设置定时初值
    TF1 = 0;		//清除TF1标志
    TR0 = 1;
    TR1 = 1;		//定时器1开始计时
    ET1 = 1;
}

void T1_ISR() interrupt 3 {
    if (++SysTick == 20) {
        TR0 = 0;
        TR1 = 0;
        SysTick = 0;
        CountFinish = 1;
    }
    if (SysTick % 10 == 0)
        UpdateRequire = 1;
}

void Delay10ms() {	//@12.000MHz
    unsigned char i, j;

    i = 117;
    j = 184;
    do {
        while (--j);
    } while (--i);
}

u8 Key_Scan() {
    P3 = 0x0f;
    if (P30 == 0) {
        Delay10ms();
        if (P30 == 0) {
            while (P30 == 0);
            return 7;
        }
    }
    if (P31 == 0) {
        Delay10ms();
        if (P31 == 0) {
            while (P31 == 0);
            return 6;
        }
    }
    if (P32 == 0) {
        Delay10ms();
        if (P32 == 0) {
            while (P32 == 0);
            return 5;
        }
    }
    if (P33 == 0) {
        Delay10ms();
        if (P33 == 0) {
            while (P33 == 0);
            return 4;
        }
    }
    return 255;
}

void Periph_Init() {
    P3M0 = P3M1 = 0;
    Select_Out (4, LEDOut);
    Select_Out (5, 0);
    Display_Init();
    Timer0Init();
    Timer1Init();
    PCF_Config (3, 102);
    EA = 1;
}

void main() {
    u8 t;
    u16 voltage16;

    Periph_Init();
    PCF_Read();

    while (1) {
        // DAC输出以及电压计算
        t = PCF_Read();
        voltage16 = (u16) (t * 5.0 / 2.55);
        if (DACStaticOut) {
            PCF_Config (3, 102);
        } else {
            PCF_Config (3, t);
        }

        // 频率计算
        if (CountFinish) {
            Frequency = TH0 * 256 + TL0;
            CountFinish = 0;
            TL1 = 0xB0;		//设置定时初值
            TH1 = 0x3C;		//设置定时初值
            TH0 = 0;
            TL0 = 0;
            TR0 = 1;
            TR1 = 1;
        }

        // 按键处理
        t = Key_Scan();
        switch (t) {
            case 4:
                Menu = Menu ? M_U : M_F;
                UpdateRequire = 1;
                break;
            case 5:
                DACStaticOut = !DACStaticOut;
                break;
            case 6:
                LEDEnable = !LEDEnable;
                break;
            case 7:
                DisplayEnable = !DisplayEnable;
                UpdateRequire = 1;
                break;
        }

        // 界面更新
        if (UpdateRequire) {
            UpdateRequire = 0;
            Display_Clear();
            if (DisplayEnable)
                switch (Menu) {
                    case M_U:
                        Display_Buffer[0] = 12;
                        Display_Buffer[5] = voltage16 / 100 + 0x10;
                        Display_Buffer[6] = (voltage16 % 100) / 10;
                        Display_Buffer[7] = voltage16 % 10;
                        break;

                    case M_F:
                        Display_Buffer[0] = 11;
                        if (Frequency >= 10000) {
                            Display_Buffer[3] = Frequency / 10000;
                        }
                        if (Frequency >= 1000) {
                            Display_Buffer[4] = (Frequency % 10000) / 1000;
                        }
                        if (Frequency >= 100) {
                            Display_Buffer[5] = (Frequency % 1000) / 100;
                        }
                        if (Frequency >= 10) {
                            Display_Buffer[6] = (Frequency % 100) / 10;
                        }
                        Display_Buffer[7] = Frequency % 10;
                        break;
                }
        }

        if (Menu == M_U)
            LEDOut = 0xfe;
        else
            LEDOut = 0xfd;
        if (voltage16 >= 350 || (voltage16 >= 150 && voltage16 < 250)) {
            LEDOut &= ~0x04;
        }
        if ((Frequency >= 1000 && Frequency <= 5000) || Frequency >= 10000) {
            LEDOut &= ~0x08;
        }
        if (!DACStaticOut) {
            LEDOut &= ~0x10;
        }
        if (!LEDEnable) {
            LEDOut = 0xff;
        }
        if (LastLEDOut != LEDOut) {
            LastLEDOut = LEDOut;
            Select_Out (4, LEDOut);
        }
    }
}
