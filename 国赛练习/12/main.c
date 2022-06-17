// 10:20 - 11:50 1h30m
// 14:50 - 17:20 2h30m
#include "main.h"
#include "display.h"
#include "ds1302.h"
#include "sonic.h"
#include "key.h"
#include "pcf.h"

u8 code capture_time_table[] = {2, 3, 5, 7, 9};

bit distance_mode = DISTANCE_MODE_C,
    update_require = 1;

u8  menu = MENU_TIME,
    capture_time_index = 0,
    distance_parameter = 20,
    show_record = 0,
    operation_capture_time_index = 0,
    operation_distance_parameter = 0,
    led = 0,
    lastLed = 0;

u16 distance_value = 0,
    distance_max = 0,
    distance_min = 65535,
    distance_avg_10 = 0;

void Select_Out (u8 x, u8 d) {
    P2 &= 0x1F;
    P0 = d;
    P2 |= (x << 5);
    _nop_();
    _nop_();
    P2 &= 0x1F;
}

void Timer0Init (void) {	//50毫秒@12.000MHz
    AUXR &= 0x7F;		//定时器时钟12T模式
    TMOD &= 0xF0;		//设置定时器模式
    TL0 = 0xB0;		//设置定时初始值
    TH0 = 0x3C;		//设置定时初始值
    TF0 = 0;		//清除TF0标志
    TR0 = 1;
    ET0 = 1;
}

void T0INT() interrupt 1 {
    static u8 cnt = 0;

    if (++cnt % 20 == 0) {
        update_require = 1;
        cnt = 0;
    }
}

void Periph_Init() {
    Display_Init();
    Select_Out (4, 0xff);
    Select_Out (5, 0);
    DS1302_Init();
    Timer0Init();
    PCF_Out_Voltage (51);
    P3M1 = P3M0 = 0;
    EA = 1;
}

void Show_Menu() {
    static u8 lastMenu = MENU_TIME;
    u16 tmp16;

    if (lastMenu != menu) {
        Display_Clear();
        if (menu == MENU_RECORD)
            show_record = 0;
        if (menu == MENU_PARAMETER) {
            show_record = 0;
            operation_capture_time_index = capture_time_index;
            operation_distance_parameter = distance_parameter;
        }
        if (lastMenu == MENU_PARAMETER) {
            capture_time_index = operation_capture_time_index;
            distance_parameter = operation_distance_parameter;
        }
        lastMenu = menu;
    }

    switch (menu) {
        case MENU_TIME:
            DispBuf[0] = read_time.hh / 10;
            DispBuf[1] = read_time.hh % 10;
            DispBuf[2] = CHAR_DEC;
            DispBuf[3] = read_time.mm / 10;
            DispBuf[4] = read_time.mm % 10;
            DispBuf[5] = CHAR_DEC;
            DispBuf[6] = read_time.ss / 10;
            DispBuf[7] = read_time.ss % 10;
            led |= 0x01;
            break;

        case MENU_DISTANCE:
            DispBuf[0] = CHAR_L;
            DispBuf[1] = distance_mode ? CHAR_C : CHAR_F;
            tmp16 = distance_value % 1000;
            if (tmp16 > 99)
                DispBuf[5] = tmp16 / 100;
            else
                DispBuf[5] = CHAR_NUL;
            DispBuf[6] = (tmp16 % 100) / 10;
            DispBuf[7] = tmp16 % 10;
            led |= 0x02;
            break;

        case MENU_RECORD:
            DispBuf[0] = CHAR_H;
            switch (show_record) {
                case 0:
                    DispBuf[1] = CHAR_TYPE_MAX;
                    DispBuf[4] = CHAR_NUL;
                    tmp16 = distance_max % 1000;
                    if (tmp16 > 99)
                        DispBuf[5] = tmp16 / 100;
                    else
                        DispBuf[5] = CHAR_NUL;
                    DispBuf[6] = (tmp16 % 100) / 10;
                    DispBuf[7] = tmp16 % 10;
                    break;

                case 1:
                    DispBuf[1] = CHAR_TYPE_MIN;
                    DispBuf[4] = CHAR_NUL;
                    tmp16 = distance_min % 1000;
                    if (distance_min == 65535)
                        tmp16 = 0;
                    if (tmp16 > 99)
                        DispBuf[5] = tmp16 / 100;
                    else
                        DispBuf[5] = CHAR_NUL;
                    DispBuf[6] = (tmp16 % 100) / 10;
                    DispBuf[7] = tmp16 % 10;

                    break;

                case 2:
                    DispBuf[1] = CHAR_TYPE_AVG;
                    tmp16 = distance_avg_10;

                    if (tmp16 > 999)
                        DispBuf[4] = tmp16 / 1000;
                    else
                        DispBuf[4] = CHAR_NUL;
                    DispBuf[5] = (tmp16 % 1000) / 100;
                    DispBuf[6] = (tmp16 % 100) / 10 + 0x10;
                    DispBuf[7] = tmp16 % 10;
                    break;
            }

            led |= 0x04;
            break;

        case MENU_PARAMETER:
            DispBuf[0] = CHAR_P;
            DispBuf[1] = show_record + 1;
            switch (show_record) {
                case 0:
                    DispBuf[6] = 0;
                    DispBuf[7] = capture_time_table[operation_capture_time_index];
                    break;

                case 1:
                    DispBuf[6] = operation_distance_parameter / 10;
                    DispBuf[7] = operation_distance_parameter % 10;
                    break;
            }
            break;
    }
}

void Update_Distance (u16 d) {
    if (d > distance_max)
        distance_max = d;
    if (d < distance_min)
        distance_min = d;
    distance_avg_10 = (u16) (((float)distance_avg_10 / 10 + d) * 5);
}

void main() {
    u8 key, light, last_light, voltage, range_count = 0;

    Periph_Init();
    Sonic_Init();
    Show_Menu();
    PCF_Out_Voltage (128);
    while (1) {
        key = Key_Scan();
        switch (key) {
            case 4:
                if (menu == MENU_PARAMETER)
                    menu = MENU_TIME;
                else
                    menu = MENU_PARAMETER;
                break;

            case 5:
                if (menu != MENU_PARAMETER) {
                    menu = ++menu % 3;
                } else if (menu != MENU_PARAMETER) {
                    show_record = ++show_record % 2;
                }
                break;

            case 8:
                if (menu == MENU_DISTANCE) {
                    distance_mode = !distance_mode;
                }
                if (menu == MENU_RECORD) {
                    show_record = ++show_record % 3;
                }
                break;

            case 9:
                if (menu == MENU_PARAMETER) {
                    if (show_record == 0) {
                        operation_capture_time_index = ++operation_capture_time_index % 5;
                    } else {
                        operation_distance_parameter += 10;
                        if (operation_distance_parameter == 90)
                            operation_distance_parameter = 10;
                    }
                }
                break;
        }
        if (update_require == 1 || key != 0) {
            led = 0;
            DS1302_Read();
            if (distance_mode == DISTANCE_MODE_F) {
                if (read_time.ss % capture_time_table[capture_time_index] == 0) {
                    distance_value = Sonic_TrigMeasure();
                    Update_Distance (distance_value);
                    if (distance_value >= (distance_parameter - 5) &&
                            distance_value <= (distance_parameter + 5))
                        range_count ++;
                    else
                        range_count = 0;
                }
            } else {
                light = PCF_Read_Channel (1);
                if (last_light >= LIGHT_THRESHOLD && light < LIGHT_THRESHOLD) {
                    distance_value = Sonic_TrigMeasure();
                    Update_Distance (distance_value);

                }
                if (light > LIGHT_THRESHOLD) {
                    led |= 0x20;
                } else {
                    led &= ~0x20;
                }
                last_light = light;
                led |= 0x08;
            }
            update_require = 0;
            Show_Menu();


            if (range_count >= 3)
                led |= 0x10;

            if (distance_value > 79)
                voltage = 255;
            else if (distance_value < 11)
                voltage = 51;
            else {
                voltage = 51 + (distance_value - 10) * 2.914;
            }
            PCF_Out_Voltage (voltage);
            if (led != lastLed) {
                Select_Out (4, ~led);
                lastLed = led;
            }
        }
    }
}