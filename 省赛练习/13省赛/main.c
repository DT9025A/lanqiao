#include "main.h"
#include "display.h"
#include "onewire.h"
#include "ds1302.h"

bit DisplayMMSS = 0,        // 是否显示分-秒界面
    WorkMode = MODE_TEMP,   // 工作模式
    DataUpdateRequire = 0;  // 数据更新标志位

u8 Menu = MENU_TEMP,        // 当前显示的界面
   TempArg = 23;            // 温度参数值

u16 TempMul10 = 0,          // 放大10倍的当前温度值
    SysTick = 0,            // 时钟刻
    S17PressedTick = 0;     // S17按下时的时钟刻

KeyStatusGroup KeyStatus = {0},     // 按键状态
               LastKeyStatus = {0}; // 上一个按键状态

PeriphControlGroup LED = {0},       // LED控制
                   Periph = {0};    // 外设控制

// 刷新显示
void Refresh_Display() {
    Display_Buffer[0] = CHAR_U;  // U
    Display_Buffer[1] = Menu;    // 显示界面参数, 1-3
    switch (Menu) {
        case MENU_TEMP:
            // 温度显示
            Display_Buffer[5] = TempMul10 / 100;
            Display_Buffer[6] = ((TempMul10 % 100) / 10) + 0x10;
            Display_Buffer[7] = TempMul10 % 10;
            break;

        case MENU_TIME:
            // 时间显示
            if (DisplayMMSS) {
                // 显示MM-SS
                Display_Buffer[3] = currentTime.mm / 10;
                Display_Buffer[4] = currentTime.mm % 10;
                Display_Buffer[5] = CHAR_DEC;
                Display_Buffer[6] = currentTime.ss / 10;
                Display_Buffer[7] = currentTime.ss % 10;
            } else {
                // 显示HH-MM
                Display_Buffer[3] = currentTime.hh / 10;
                Display_Buffer[4] = currentTime.hh % 10;
                Display_Buffer[5] = CHAR_DEC;
                Display_Buffer[6] = currentTime.mm / 10;
                Display_Buffer[7] = currentTime.mm % 10;
            }
            break;

        case MENU_ARG:
            // 显示参数
            Display_Buffer[6] = TempArg / 10;
            Display_Buffer[7] = TempArg % 10;
            break;
    }
    // 判断当前状态与上一状态是否相同，不同则更新锁存数据
    if (LED.LastStatus != LED.Status) {
        // 更新LED状态
        LED.LastStatus = LED.Status;
        Select_Out (4, ~LED.Status);  // 因为LED共阳，程序内控制逻辑为1亮，故按位取反
    }
    if (Periph.LastStatus != Periph.Status) {
        // 更新外设状态
        Periph.LastStatus = Periph.Status;
        Select_Out (5, Periph.Status);
    }
}

// 主要控制逻辑实现
void Logic_Control() {
    // 清空外设与LED1的状态
    Periph.Status &= ~ (1 << 4);
    LED.Status &= ~1;

    if (WorkMode == MODE_TEMP) {
        // 温度模式，L2置1
        LED.Status |= 0x02;
        if (TempMul10 > TempArg * 10) {
            // 温度满足控制条件，开继电器
            Periph.Status |= 1 << 4;
        }
    } else {
        // 时间模式，清除L2状态
        LED.Status &= ~0x02;
    }

    // 在整点的0-5秒范围内
    if (currentTime.mm == 0 && currentTime.ss < 5) {
        if (WorkMode == MODE_TIME)
            // 时间模式，开继电器
            Periph.Status |= 1 << 4;
        // 开L1
        LED.Status |= 1;
    }
}

// 25ms时钟中断
void T0_ISR() interrupt 1 {
    // SysTick限制范围
    if (++SysTick == 60000)
        SysTick = 0;

    // 这点写错了其实，但对于整体功能没啥影响
    /*
    if (LastKeyStatus.S17 == 1 && KeyStatus.S17 == 1)
        S17PressedTick += 1;
    else
        S17PressedTick = 0;
    */

    // 100ms
    if (SysTick % 4 == 0) {
        // 更新数据
        DataUpdateRequire = 1;
        // 判断是否需要闪烁L3
        if (Periph.Status & 0x10) {
            // 异或，状态取反
            LED.Status ^= 0x04;
        } else {
            // 清空状态
            LED.Status &= ~0x04;
        }
    }
}

// 计算两个Tick之间的间隔
u16 TickDistance (u16 tick) {
    if (SysTick < tick)
        return ((unsigned long)SysTick + 65536 - tick);
    return SysTick - tick;
}

void main() {
    u16 tmp;
    Periph_Init();

    while (1) {
        // 如果需要更新数据
        if (DataUpdateRequire) {
            DataUpdateRequire = 0;
            // 对初始读到的85度做一下限制
            if ((tmp = rd_temperature() * 10) >= 850) {
                if (TempMul10 > 800)
                    TempMul10 = tmp;
            } else {
                TempMul10 = tmp;
            }
            // 读取时间
            DS1302_Read_Current_Time();
        }
        // 按键扫描
        Key_Scan();
        if (LastKeyStatus.S12 == 0 && KeyStatus.S12) {
            // S12单次按下，更换当前显示界面，清空显示缓冲区
            Display_Clear();
            // 更新一下数据
            DataUpdateRequire = 1;
            // Menu取值：1-3
            if (++Menu == MENU_ARG + 1)
                Menu = MENU_TEMP;
        } else if (LastKeyStatus.S13 == 0 && KeyStatus.S13) {
            // S13单次按下，改变工作模式
            WorkMode = !WorkMode;
        } else if (LastKeyStatus.S16 == 0 && KeyStatus.S16) {
            // S16单次按下，参数加一，最大限制99
            if (Menu == MENU_ARG) {
                if (TempArg < 99)
                    TempArg += 1;
            }
        } else if (LastKeyStatus.S17 == 0 && KeyStatus.S17) {
            // S17单次按下，参数减一，最小限制10
            // 因为S17有长按功能，在这里记录一下Tick
            S17PressedTick = SysTick;
            if (Menu == MENU_ARG) {
                if (TempArg > 10)
                    TempArg -= 1;
            }
        } else if (LastKeyStatus.S17 && KeyStatus.S17) {
            // S17连续两次检测均为按下，判断一下是否长按
            if (TickDistance (S17PressedTick) > 500) {
                // 显示M-S
                DisplayMMSS = 1;
            }
        } else {
            // 显示H-M
            DisplayMMSS = 0;
        }
        // 逻辑控制与显示刷新
        Logic_Control();
        Refresh_Display();
    }
}