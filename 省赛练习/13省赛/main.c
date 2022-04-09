#include "main.h"
#include "display.h"
#include "onewire.h"
#include "ds1302.h"

bit DisplayMMSS = 0,        // �Ƿ���ʾ��-�����
    WorkMode = MODE_TEMP,   // ����ģʽ
    DataUpdateRequire = 0;  // ���ݸ��±�־λ

u8 Menu = MENU_TEMP,        // ��ǰ��ʾ�Ľ���
   TempArg = 23;            // �¶Ȳ���ֵ

u16 TempMul10 = 0,          // �Ŵ�10���ĵ�ǰ�¶�ֵ
    SysTick = 0,            // ʱ�ӿ�
    S17PressedTick = 0;     // S17����ʱ��ʱ�ӿ�

KeyStatusGroup KeyStatus = {0},     // ����״̬
               LastKeyStatus = {0}; // ��һ������״̬

PeriphControlGroup LED = {0},       // LED����
                   Periph = {0};    // �������

// ˢ����ʾ
void Refresh_Display() {
    Display_Buffer[0] = CHAR_U;  // U
    Display_Buffer[1] = Menu;    // ��ʾ�������, 1-3
    switch (Menu) {
        case MENU_TEMP:
            // �¶���ʾ
            Display_Buffer[5] = TempMul10 / 100;
            Display_Buffer[6] = ((TempMul10 % 100) / 10) + 0x10;
            Display_Buffer[7] = TempMul10 % 10;
            break;

        case MENU_TIME:
            // ʱ����ʾ
            if (DisplayMMSS) {
                // ��ʾMM-SS
                Display_Buffer[3] = currentTime.mm / 10;
                Display_Buffer[4] = currentTime.mm % 10;
                Display_Buffer[5] = CHAR_DEC;
                Display_Buffer[6] = currentTime.ss / 10;
                Display_Buffer[7] = currentTime.ss % 10;
            } else {
                // ��ʾHH-MM
                Display_Buffer[3] = currentTime.hh / 10;
                Display_Buffer[4] = currentTime.hh % 10;
                Display_Buffer[5] = CHAR_DEC;
                Display_Buffer[6] = currentTime.mm / 10;
                Display_Buffer[7] = currentTime.mm % 10;
            }
            break;

        case MENU_ARG:
            // ��ʾ����
            Display_Buffer[6] = TempArg / 10;
            Display_Buffer[7] = TempArg % 10;
            break;
    }
    // �жϵ�ǰ״̬����һ״̬�Ƿ���ͬ����ͬ�������������
    if (LED.LastStatus != LED.Status) {
        // ����LED״̬
        LED.LastStatus = LED.Status;
        Select_Out (4, ~LED.Status);  // ��ΪLED�����������ڿ����߼�Ϊ1�����ʰ�λȡ��
    }
    if (Periph.LastStatus != Periph.Status) {
        // ��������״̬
        Periph.LastStatus = Periph.Status;
        Select_Out (5, Periph.Status);
    }
}

// ��Ҫ�����߼�ʵ��
void Logic_Control() {
    // ���������LED1��״̬
    Periph.Status &= ~ (1 << 4);
    LED.Status &= ~1;

    if (WorkMode == MODE_TEMP) {
        // �¶�ģʽ��L2��1
        LED.Status |= 0x02;
        if (TempMul10 > TempArg * 10) {
            // �¶�����������������̵���
            Periph.Status |= 1 << 4;
        }
    } else {
        // ʱ��ģʽ�����L2״̬
        LED.Status &= ~0x02;
    }

    // �������0-5�뷶Χ��
    if (currentTime.mm == 0 && currentTime.ss < 5) {
        if (WorkMode == MODE_TIME)
            // ʱ��ģʽ�����̵���
            Periph.Status |= 1 << 4;
        // ��L1
        LED.Status |= 1;
    }
}

// 25msʱ���ж�
void T0_ISR() interrupt 1 {
    // SysTick���Ʒ�Χ
    if (++SysTick == 60000)
        SysTick = 0;

    // ���д������ʵ�����������幦��ûɶӰ��
    /*
    if (LastKeyStatus.S17 == 1 && KeyStatus.S17 == 1)
        S17PressedTick += 1;
    else
        S17PressedTick = 0;
    */

    // 100ms
    if (SysTick % 4 == 0) {
        // ��������
        DataUpdateRequire = 1;
        // �ж��Ƿ���Ҫ��˸L3
        if (Periph.Status & 0x10) {
            // ���״̬ȡ��
            LED.Status ^= 0x04;
        } else {
            // ���״̬
            LED.Status &= ~0x04;
        }
    }
}

// ��������Tick֮��ļ��
u16 TickDistance (u16 tick) {
    if (SysTick < tick)
        return ((unsigned long)SysTick + 65536 - tick);
    return SysTick - tick;
}

void main() {
    u16 tmp;
    Periph_Init();

    while (1) {
        // �����Ҫ��������
        if (DataUpdateRequire) {
            DataUpdateRequire = 0;
            // �Գ�ʼ������85����һ������
            if ((tmp = rd_temperature() * 10) >= 850) {
                if (TempMul10 > 800)
                    TempMul10 = tmp;
            } else {
                TempMul10 = tmp;
            }
            // ��ȡʱ��
            DS1302_Read_Current_Time();
        }
        // ����ɨ��
        Key_Scan();
        if (LastKeyStatus.S12 == 0 && KeyStatus.S12) {
            // S12���ΰ��£�������ǰ��ʾ���棬�����ʾ������
            Display_Clear();
            // ����һ������
            DataUpdateRequire = 1;
            // Menuȡֵ��1-3
            if (++Menu == MENU_ARG + 1)
                Menu = MENU_TEMP;
        } else if (LastKeyStatus.S13 == 0 && KeyStatus.S13) {
            // S13���ΰ��£��ı乤��ģʽ
            WorkMode = !WorkMode;
        } else if (LastKeyStatus.S16 == 0 && KeyStatus.S16) {
            // S16���ΰ��£�������һ���������99
            if (Menu == MENU_ARG) {
                if (TempArg < 99)
                    TempArg += 1;
            }
        } else if (LastKeyStatus.S17 == 0 && KeyStatus.S17) {
            // S17���ΰ��£�������һ����С����10
            // ��ΪS17�г������ܣ��������¼һ��Tick
            S17PressedTick = SysTick;
            if (Menu == MENU_ARG) {
                if (TempArg > 10)
                    TempArg -= 1;
            }
        } else if (LastKeyStatus.S17 && KeyStatus.S17) {
            // S17�������μ���Ϊ���£��ж�һ���Ƿ񳤰�
            if (TickDistance (S17PressedTick) > 500) {
                // ��ʾM-S
                DisplayMMSS = 1;
            }
        } else {
            // ��ʾH-M
            DisplayMMSS = 0;
        }
        // �߼���������ʾˢ��
        Logic_Control();
        Refresh_Display();
    }
}