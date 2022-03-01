#include "main.h"
#include "display.h"
#include "pcf.h"
#include "at24.h"

bit UpdateRequire = 1, VoltageHelding = 0;
u8 VoltageArgU8 = 0, DisplayMenu = M_U, VoltageHeldTime = 0;
u16 VoltageCount = 0, VoltageU16;
float VoltageArg = 0.0f, Voltage = 0.0f, LastVoltage = 0.0f;

void Select_Out (u8 x, u8 dat) {
    P2 &= 0x1f;
    P2 |= (x << 5);
    P0 = dat;
    _nop_();
    P2 &= 0x1f;
    P0 = 0;
}

void Periph_Init() {
    P3M0 = P3M1 = 0;
    Select_Out (4, 0xff);
    Select_Out (5, 0);
}

void Timer0Init (void) {	//25毫秒@12.000MHz
    AUXR &= 0x7F;		//定时器时钟12T模式
    TMOD &= 0xF0;		//设置定时器模式
    TL0 = 0x58;		//设置定时初值
    TH0 = 0x9E;		//设置定时初值
    TF0 = 0;		//清除TF0标志
    TR0 = 1;		//定时器0开始计时
    ET0 = 1;
}

void T0_ISR() interrupt 1 {
    static u8 i = 0;

    if (++i == 10) {
        UpdateRequire = 1;
        i = 0;
    }
    if (VoltageHelding) {
        VoltageHeldTime += 1;
    }
}

void Delay10ms() {	//@12.000MHz
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
    P3 = 0x0f;
    P42 = 1;
    P44 = 1;

    if (P3 != 0x0f) {
        Delay10ms();
        if (P3 != 0x0f) {
            P34 = 1;
            P35 = 0;
            if (P32 == 0) {
                if (P32 == 0) {
                    while (P32 == 0);
                    return 13;
                }
            }
            if (P33 == 0) {
                Delay10ms();
                if (P33 == 0) {
                    while (P33 == 0);
                    return 12;
                }
            }
            P34 = 0;
            P35 = 1;
            if (P32 == 0) {
                if (P32 == 0) {
                    while (P32 == 0);
                    return 17;
                }
            }
            if (P33 == 0) {
                Delay10ms();
                if (P33 == 0) {
                    while (P33 == 0);
                    return 16;
                }
            }
        }
    }
    return 255;
}

void main() {
    u8 key, led = 0, lastLED = 0, uselessKey = 0;

    Periph_Init();
    Display_Init();
    PCF_Init();
    PCF_Read();
    Timer0Init();
    EA = 1;

    VoltageArgU8 = AT24C02_ReadByte (0);
    VoltageArg = (float)VoltageArgU8 / 10;
    while (1) {
        key = Key_Scan();
        switch (key) {
            case 12:
                if (DisplayMenu == M_N)
                    DisplayMenu = M_U;
                else
                    DisplayMenu += 1;
                if (DisplayMenu == M_N) {
                    VoltageArg = (float)VoltageArgU8 / 10;
                    AT24C02_WriteByte (0, VoltageArgU8);
                }
                Display_Clear();
                uselessKey = 0;
                break;

            case 13:
                if (DisplayMenu == M_N) {
                    VoltageCount = 0;
                    uselessKey = 0;
                } else {
                    uselessKey += 1;
                }
                break;

            case 16:
                if (DisplayMenu == M_P) {
                    uselessKey = 0;
                    UpdateRequire = 1;
                    if (VoltageArgU8 <= 45)
                        VoltageArgU8 += 5;
                    else
                        VoltageArgU8 = 0;
                } else {
                    uselessKey += 1;
                }
                break;

            case 17:
                if (DisplayMenu == M_P) {
                    uselessKey = 0;
                    UpdateRequire = 1;
                    if (VoltageArgU8 > 4)
                        VoltageArgU8 -= 5;
                    else
                        VoltageArgU8 = 50;
                } else {
                    uselessKey += 1;
                }
                break;
        }

        Voltage = (float)PCF_Read() / 51;
        if (LastVoltage > VoltageArg && VoltageArg >= Voltage) {
            VoltageCount += 1;
        }
        LastVoltage = Voltage;
        VoltageU16 = (u16) (Voltage * 100);

        if (VoltageArg >= Voltage) {
            VoltageHelding = 1;
        } else {
            VoltageHelding = 0;
            VoltageHeldTime = 0;
        }

        if (VoltageHeldTime >= 200) {
            VoltageHelding = 0;
            led = 0xfe;
        } else
            led = 0xff;
        if (VoltageCount % 2 == 1)
            led &= ~0x02;
        if (uselessKey >= 3)
            led &= ~0x04;
        if (lastLED != led)
            Select_Out (4, (lastLED = led));

        if (UpdateRequire) {
            UpdateRequire = 0;
            switch (DisplayMenu) {
                case M_U:
                    Display_Buffer[0] = 11;
                    Display_Buffer[5] = VoltageU16 / 100 + 0x10;
                    VoltageU16 %= 100 ;
                    Display_Buffer[6] = VoltageU16 / 10;
                    Display_Buffer[7] = VoltageU16 % 10;
                    break;

                case M_P:
                    Display_Buffer[0] = 12;
                    Display_Buffer[5] = (VoltageArgU8 % 100) / 10 + 0x10;
                    Display_Buffer[6] = VoltageArgU8 % 10;
                    Display_Buffer[7] = 0;
                    break;

                case M_N:
                    Display_Buffer[0] = 13;
                    if (VoltageCount >= 10000)
                        Display_Buffer[3] = VoltageCount / 10000;
                    else
                        Display_Buffer[3] = 10;
                    if (VoltageCount >= 1000)
                        Display_Buffer[4] = (VoltageCount % 10000) / 1000;
                    else
                        Display_Buffer[4] = 10;
                    if (VoltageCount >= 100)
                        Display_Buffer[5] = (VoltageCount % 1000) / 100;
                    else
                        Display_Buffer[5] = 10;
                    if (VoltageCount >= 10)
                        Display_Buffer[6] = (VoltageCount % 100) / 10;
                    else
                        Display_Buffer[6] = 10;
                    Display_Buffer[7] = VoltageCount % 10;
                    break;
            }
        }
    }
}
