/*
  程序说明: 单总线驱动程序
  软件环境: Keil uVision 4.10
  硬件环境: CT107单片机综合实训平台(外部晶振12MHz) STC89C52RC单片机
  日    期: 2011-8-9
*/
#include <STC15.H>
#include <intrins.h>

sbit DQ = P1 ^ 4; //单总线接口


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


//通过单总线向DS18B20写一个字节
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

//从DS18B20读取一个字节
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

//DS18B20设备初始化
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






