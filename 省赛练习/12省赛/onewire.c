/*
  ����˵��: ��������������
  �������: Keil uVision 4.10
  Ӳ������: CT107��Ƭ���ۺ�ʵѵƽ̨(�ⲿ����12MHz) STC89C52RC��Ƭ��
  ��    ��: 2011-8-9
*/
#include <STC15.H>
#include <intrins.h>

sbit DQ = P1 ^ 4; //�����߽ӿ�


void Delay500us() {	//@12.000MHz
    unsigned char i, j;

    i = 6;
    j = 211;
    do {
        while (--j);
    } while (--i);
}

void Delay2us() {	//@12.000MHz
    unsigned char i;

    _nop_();
    _nop_();
    i = 3;
    while (--i);
}

void Delay10us() {	//@12.000MHz
    unsigned char i;

    _nop_();
    _nop_();
    i = 27;
    while (--i);
}

void Delay80us() {	//@12.000MHz
    unsigned char i, j;

    i = 1;
    j = 236;
    do {
        while (--j);
    } while (--i);
}


//ͨ����������DS18B20дһ���ֽ�
void Write_DS18B20 (unsigned char dat) {
    unsigned char i;
    for (i = 0; i < 8; i++) {
        DQ = 0;
        Delay2us();
        DQ = dat & 0x01;
        Delay10us();
        Delay10us();
        Delay10us();
        Delay10us();
        Delay10us();
        Delay10us();
        DQ = 1;
        dat >>= 1;
    }
    Delay10us();
    Delay10us();
}

//��DS18B20��ȡһ���ֽ�
unsigned char Read_DS18B20 (void) {
    unsigned char i;
    unsigned char dat;

    for (i = 0; i < 8; i++) {
        DQ = 0;
        dat >>= 1;
        Delay2us();
        DQ = 1;
        if (DQ) {
            dat |= 0x80;
        }
        Delay10us();
        Delay10us();
        Delay10us();
        Delay10us();
        Delay10us();
        Delay10us();
    }
    return dat;
}

//DS18B20�豸��ʼ��
bit Init_DS18B20 (void) {
    bit initflag = 0;

    DQ = 1;
    Delay10us();
    DQ = 0;
    Delay500us();
    DQ = 1;
    Delay80us();
    initflag = DQ;
    Delay80us();
    Delay80us();
    Delay80us();

    return initflag;
}






