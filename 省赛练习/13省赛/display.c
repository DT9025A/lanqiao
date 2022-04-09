#include "display.h"

// ��ģ
u8 Tab[0x20] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0xff, 0xc1, 0xbf};
// ��ʾ������
u8 Display_Buffer[8];

void Display_Clear() {
    // �����������ȫ��䲻������ģ�±�
    memset (Display_Buffer, 10, sizeof (Display_Buffer));
}

void Timer2Init (void) {	//2����@12.000MHz
    AUXR |= 0x04;		//��ʱ��ʱ��1Tģʽ
    T2L = 0x40;		//���ö�ʱ��ֵ
    T2H = 0xA2;		//���ö�ʱ��ֵ
    AUXR |= 0x10;		//��ʱ��2��ʼ��ʱ
    IE2 |= 0x04;
}

void Display_Init() {
    u8 i;
    // �������С�������ģ
    for (i = 0; i < 10; i++)
        Tab[0x10 + i] = Tab[i] & ~0x80;
    // ���������
    Display_Clear();
    // ��ʼ����ʱ��2
    Timer2Init();
}

// ��ʱ��2�жϴ������������������ܽ�����ʾɨ��
void T2_ISR() interrupt 12 {
    static u8 i = 0;

    // ѡ�񵥸������
    Select_Out (6, 1 << i);
    // ����ʾ����
    Select_Out (7, Tab[Display_Buffer[i]]);

    // �����±�߽�
    if (++i == 8)
        i = 0;
}
