#include "main.h"
#include "display.h"
#include "onewire.h"
#include "ds1302.h"

// ��main.c�ı���
extern KeyStatusGroup KeyStatus, LastKeyStatus;

// ��ָ��λ���������
void Select_Out (u8 x, u8 d) {
    P2 &= 0x1F;
    P0 = d;
    P2 |= (x << 5);
    _nop_();
    P2 &= 0x1F;
}

void Delay100ms() {	//@12.000MHz
    unsigned char i, j, k;

    _nop_();
    _nop_();
    i = 5;
    j = 144;
    k = 71;
    do {
        do {
            while (--k);
        } while (--j);
    } while (--i);
}

// ��ʼ������
void Periph_Init() {
    // �ص�
    Select_Out (4, 0xff);
    // ������
    Select_Out (5, 0);
    // ��ʼ���������ʾ
    Display_Init();
    // ��ʼ��DS1302
    DS1302_Init();
    // ��ǰ����һ���¶�ת����������ûɶ��
    rd_temperature();
    // �ȴ�ת��
    Delay100ms();
    // ��ʼ��T0
    Timer0Init();
    // �����ж�
    EA = 1;
}

void Delay10ms() {	//@12.000MHz
    unsigned char i, j;

    i = 117;
    j = 184;
    do {
        while (--j);
    } while (--i);
}

void Timer0Init (void) {	//25����@12.000MHz
    AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
    TMOD &= 0xF0;		//���ö�ʱ��ģʽ
    TL0 = 0x58;		//���ö�ʱ��ֵ
    TH0 = 0x9E;		//���ö�ʱ��ֵ
    TF0 = 0;		//���TF0��־
    TR0 = 1;		//��ʱ��0��ʼ��ʱ
    ET0 = 1;
}

void Key_Scan() {
    // ��ʼ��P3״̬Ϊȫ1
    P3 = 0xFF;

    //������һ��״̬�����㵱ǰ״̬
    memcpy (&LastKeyStatus, &KeyStatus, sizeof (KeyStatusGroup));
    memset (&KeyStatus, 0, sizeof (KeyStatusGroup));

    // ������ǻ����ľ���ɨ��
    P34 = 0;
    if (P32 == 0) {
        Delay10ms();
        if (P32 == 0) {
            KeyStatus.S17 = 1;
            return;
        }
    }
    if (P33 == 0) {
        Delay10ms();
        if (P33 == 0) {
            KeyStatus.S16 = 1;
            return;
        }
    }
    P34 = 1;
    P35 = 0;
    if (P32 == 0) {
        Delay10ms();
        if (P32 == 0) {
            KeyStatus.S13 = 1;
            return;
        }
    }
    if (P33 == 0) {
        Delay10ms();
        if (P33 == 0) {
            KeyStatus.S12 = 1;
            return;
        }
    }
}
