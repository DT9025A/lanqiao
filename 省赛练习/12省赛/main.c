#include <STC15.H>
#include <intrins.h>

#include "display.h"
#include "ds18b20.h"
#include "pcf8591.h"

// 指定当前界面的枚举
enum {
    M_TEMP,
    M_PARAM,
    M_DAC
};

// 当前界面 
unsigned char menu = M_TEMP;
// 当前输出模式
unsigned char mode = 1;
// 当前温度参数
unsigned char tempArg = 25;
// 临时温度参数
unsigned char tmpTempArg;
// 需要更新界面
bit UpdateRequire;

// 向指定位置输出数据
void Select_Out (unsigned char x, unsigned char dat) {
    P2 &= 0x1F;
    P2 |= (x << 5);
    P0 = dat;
    _nop_();
    P2 &= 0x1F;
    P0 = 0;
}

// 初始化外设
void Periph_Init() {
    P3M0 = P3M1 = 0;
    Select_Out (4, 0xff);
    Select_Out (5, 0x00);
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

// 500毫秒更新一次界面
void Timer0_ISR() interrupt 1 {
    static unsigned char cnt = 0;
    if (++cnt == 10) {
        UpdateRequire = 1;
        cnt = 0;
    }
}

// 消抖用
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

// 按键扫描函数
unsigned char Key_Scan (bit wait) {
    P44 = 0;
    P42 = 1;
    P3 = 0xFF;
    if (P33 == 0) {
        Delay20ms();
        if (P33 == 0) {
            while (P33 == 0 && wait == 1);
            return 4;
        }
    }
    if (P32 == 0) {
        Delay20ms();
        if (P32 == 0) {
            while (P32 == 0 && wait == 1);
            return 5;
        }
    }
    P44 = 1;
    P42 = 0;
    P3 = 0xFF;
    if (P33 == 0) {
        Delay20ms();
        if (P33 == 0) {
            while (P33 == 0 && wait == 1);
            return 8;
        }
    }
    if (P32 == 0) {
        Delay20ms();
        if (P32 == 0) {
            while (P32 == 0 && wait == 1);
            return 9;
        }
    }
    return 255;
}

// 通过温度计算DAC以及电压值
void Calc_DAC_Temp (float temp, float *voltage, unsigned char *dac) {
    if (mode == 1) {
        if (temp < tempArg) {
            *dac = 0;
            *voltage = 0.0f;
        } else {
            *dac = 255;
            *voltage = 5.0f;
        }
    } else {
        if (temp < 20.0f) {
            *dac = 51;
            *voltage = 1.0f;
        } else if (temp > 40.0f) {
            *dac = 204;
            *voltage = 4.0f;
        } else {
            temp -= 20.0f;
            temp *= 0.15f;
            *voltage = 1.0f + temp;
            *dac = 51 * (unsigned char)voltage;
        }
    }
}

void main() {
    float t, voltage;
    unsigned int ti;
    unsigned char key, dac, led;

    Periph_Init();
    Display_Init();
    Timer0Init();
    DS18B20_ReadTemp();

    EA = 1;
    UpdateRequire = 1;

    while (1) {

        // 按键处理
        key = Key_Scan (1);
        switch (key) {
            case 4:
                if (menu == M_DAC)
                    menu = M_TEMP;
                else
                    menu += 1;

                if (menu == M_PARAM) {
                    tmpTempArg = tempArg;
                } else if (menu == M_DAC) {
                    tempArg = tmpTempArg;
                }

                Display_Clear();
                break;

            case 5:
                if (mode == 1)
                    mode = 2;
                else
                    mode = 1;
                break;

            case 8:
                if (menu == M_PARAM) {
                    if (tmpTempArg > 0)
                        tmpTempArg -= 1;
                }
                break;

            case 9:
                if (menu == M_PARAM) {
                    if (tmpTempArg < 100)
                        tmpTempArg += 1;
                }
                break;
        }

        // 读取温度以及计算输出
        t = DS18B20_ReadTemp();
        Calc_DAC_Temp (t, &voltage, &dac);
        PCF8591_Config (0x40, dac);

        // 显示更新
        if (UpdateRequire) {
            UpdateRequire = 0;
            if(mode == 1)
                led = 0xfe;
            else
                led = 0xff;
            switch (menu) {
                case M_TEMP:
                    led &= ~0x02;
                    if (t < 60) {
                        Display_Buffer[0] = 11;
                        ti = (t * 100);
                        Display_Buffer[4] = ti / 1000;
                        ti %= 1000;
                        Display_Buffer[5] = ti / 100 + 0x10;
                        ti %= 100;
                        Display_Buffer[6] = ti / 10;
                        ti %= 10;
                        Display_Buffer[7] = ti;
                    } else {
                        UpdateRequire = 1;
                    }
                    break;

                case M_PARAM:
                    led &= ~0x04;
                    Display_Buffer[0] = 12;
                    Display_Buffer[6] = tmpTempArg / 10;
                    Display_Buffer[7] = tmpTempArg % 10;
                    UpdateRequire = 1;
                    break;

                case M_DAC:
                    led &= ~0x08;
                    Display_Buffer[0] = 13;
                    ti = voltage * 100;
                    Display_Buffer[5] = ti / 100 + 0x10;
                    ti %= 100;
                    Display_Buffer[6] = ti / 10;
                    ti %= 10;
                    Display_Buffer[7] = ti;
                    UpdateRequire = 1;
                    break;
            }
            
            Select_Out(4, led);
        }
    }
}
