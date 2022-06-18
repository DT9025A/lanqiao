#include "main.h"
#include "display.h"
#include "sonic.h"
#include "pcf.h"
#include "at24.h"
#include "key.h"
#include "ui.h"
#include "freq.h"

bit UIUpdate = 1, E2PROMUpdate = 0;

u8 ArgHumidity = 40,    //湿度参数(%，10-60)
   ArgDistance = 60,    //距离参数(CM，10-120)
   LEDStatus = 0,
   LastLEDStatus = 0xff,
   RelayCount = 0;

u16 ArgFrequency = 9000;  //频率参数(Hz，1000-12000)   

void Timer1Init (void) {	//2毫秒@12.000MHz
    AUXR |= 0x40;		//定时器时钟1T模式
    TMOD &= 0x0F;		//设置定时器模式
    TL1 = 0x40;		//设置定时初始值
    TH1 = 0xA2;		//设置定时初始值
    TF1 = 0;		//清除TF1标志
    TR1 = 1;		//定时器1开始计时
    ET1 = 1;
}

void Timer2Init (void) {	//200微秒@12.000MHz
    AUXR |= 0x04;		//定时器时钟1T模式
    T2L = 0xA0;		//设置定时初始值
    T2H = 0xF6;		//设置定时初始值
    AUXR |= 0x10;		//定时器2开始计时
    IE2 |= 0x04;
}

void Timer1ISR() interrupt 3 {
    static u16 SysTick = 0;

    SysTick += 1;
    // 限制值域
    if (SysTick == 59999)
        SysTick = 0;
    // 1秒回调
    if (SysTick % 500 == 0)
        Freq_Callback_1s();
    // 0.25秒回调
    if (SysTick % 125 == 0)
        UIUpdate = 1;
    // 0.1秒回调
    if (SysTick % 50 == 0) {
        LEDStatus = 0;
        if (UICurrentMenu == MENU_ARGU) {
            LEDStatus = 1 << (UICurrentArg - 1);
            LEDStatus &= LastLEDStatus;
            LEDStatus ^= 1 << (UICurrentArg - 1);
        } else {
            LEDStatus = 1 << UICurrentMenu;
        }
        if (UIDispFrequency > ArgFrequency)
            LEDStatus |= 0x08;
        if (UIDispHumidity > ArgHumidity)
            LEDStatus |= 0x10;
        if (UIDispDistance > ArgDistance)
            LEDStatus |= 0x20;
    }
    // 0.05秒回调
    if (SysTick % 25 == 0)
        Key_Scan();
    Display_Refresh();
}

// 200us
void Timer2ISR() interrupt 12 {
    static u8 cycle = 0;
    static bit lastRelay = 0;
    u8 PeriphStatus = 0;

    if (UIDispDistance > ArgDistance) {
        PeriphStatus |= RELAY;
        if (!lastRelay) {
            RelayCount += 1;
            lastRelay = 1;
            E2PROMUpdate = 1;
        }
    } else {
        PeriphStatus &= ~RELAY;
        lastRelay = 0;
    }

    if (++cycle == 1)
        PeriphStatus |= MOTOR;
    else {
        if (UIDispFrequency > ArgFrequency && cycle <= 4)
            PeriphStatus |= MOTOR;
        else
            PeriphStatus &= ~MOTOR;
    }
    if (cycle == 5)
        cycle = 0;
    Select_Out (5, PeriphStatus);
}

void Periph_Init() {
    P3M0 = P3M1 = 0;
    Select_Out (4, 0xff);
    Select_Out (5, 0);
    Display_Init();
    Freq_Init();
    Timer1Init();
    Timer2Init();
    EA = 1;
}

void main() {
    u8 lastSonicValue = 0;
    Periph_Init();

    while (1) {
        if (K4)  {
            UICurrentMenu = UICurrentMenu + 1;
            UICurrentMenu %= 4;
            UIUpdate = 1;
            K4 = 0;
        } else if (K5) {
            if (UICurrentMenu == MENU_ARGU) {
                UICurrentArg += 1;
                if (UICurrentArg == 4)
                    UICurrentArg = 1;
            }
            UIUpdate = 1;
            K5 = 0;
        } else if (K6) {
            if (UICurrentMenu == MENU_ARGU) {
                switch (UICurrentArg) {
                    case 1:
                        opArgFrequency += 500;
                        if (opArgFrequency == 12500)
                            opArgFrequency = 1000;
                        break;

                    case 2:
                        opArgHumidity += 10;
                        if (opArgHumidity == 70)
                            opArgHumidity = 10;
                        break;

                    case 3:
                        opArgDistance += 10;
                        if (opArgDistance == 130)
                            opArgDistance = 10;
                        break;

                    default:
                        break;
                }
            }
            UIUpdate = 1;
            K6 = 0;
        } else if (K7) {
            if (UICurrentMenu == MENU_ARGU) {
                switch (UICurrentArg) {
                    case 1:
                        opArgFrequency -= 500;
                        if (opArgFrequency == 500)
                            opArgFrequency = 12000;
                        break;

                    case 2:
                        opArgHumidity -= 10;
                        if (opArgHumidity == 0)
                            opArgHumidity = 60;
                        break;

                    case 3:
                        opArgDistance -= 10;
                        if (opArgDistance == 0)
                            opArgDistance = 120;
                        break;

                    default:
                        break;
                }
            }

            UIUpdate = 1;
            K7 = 0;
        }
        if (UICurrentMenu == MENU_DIST && K6LongPress) {
            Display_Clear();
            UIDistDisplay = !UIDistDisplay;
            K6LongPress = 0;
            UIUpdate = 1;
        }
        if (UICurrentMenu == MENU_FREQ && K7LongPress) {
            Display_Clear();
            K7LongPress = 0;
            UIFreqDisplay = !UIFreqDisplay;
            UIUpdate = 1;
        }
        if (UICurrentMenu == MENU_HUMD && K7LongPress) {
            RelayCount = 0;
            E2PROMUpdate = 1;
        }
        // 平缓的湿度输出
        UIDispHumidity = PCF_Channel_Measure (3) * 0.3922;
        if (UIDispHumidity <= ArgHumidity)
            PCF_Voltage_Out (51);
        else if (UIDispHumidity >= 80)
            PCF_Voltage_Out (255);
        else
            PCF_Voltage_Out (51 + 204.0 / (80 - ArgHumidity) * (UIDispHumidity - ArgHumidity));

        if (UIUpdate) {
            UIUpdate = 0;
            UIDispFrequency = Freq_Result;
            EA = 0;
            UIDispDistance = Sonic_Measure();
            EA = 1;
            UI_Update();
        }

        if (LastLEDStatus != LEDStatus) {
            Select_Out (4, ~LEDStatus);
            LastLEDStatus = LEDStatus;
        }

        if(E2PROMUpdate) {
            E2PROMUpdate = 0;
            AT24_Write(0, RelayCount);
        }
    }
}


void Select_Out (u8 x, u8 dat) {
    EA = 0;
    P2 &= 0x1f;
    P0 = dat;
    P2 |= (x << 5);
    _nop_();
    P2 &= 0x1f;
    EA = 1; 
}
