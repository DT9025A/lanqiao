#include "ds1302.h"

TIME currentTime;

void Write_Ds1302 (unsigned  char temp) {
    unsigned char i;
    for (i = 0; i < 8; i++) {
        SCK = 0;
        SDA = temp & 0x01;
        temp >>= 1;
        SCK = 1;
    }
}

void Write_Ds1302_Byte ( unsigned char address, unsigned char dat ) {
    RST = 0;
    _nop_();
    SCK = 0;
    _nop_();
    RST = 1;
    _nop_();
    Write_Ds1302 (address);
    Write_Ds1302 (dat);
    RST = 0;
}

unsigned char Read_Ds1302_Byte ( unsigned char address ) {
    unsigned char i, temp = 0x00;
    RST = 0;
    _nop_();
    SCK = 0;
    _nop_();
    RST = 1;
    _nop_();
    Write_Ds1302 (address);
    for (i = 0; i < 8; i++) {
        SCK = 0;
        temp >>= 1;
        if (SDA)
            temp |= 0x80;
        SCK = 1;
    }
    RST = 0;
    _nop_();
    SCK = 0;
    _nop_();
    SCK = 1;
    _nop_();
    SDA = 0;
    _nop_();
    SDA = 1;
    _nop_();
    return (temp);
}

// 初始化DS1302，对着手册抄
void DS1302_Init() {
    Write_Ds1302_Byte (0x8e, 0);
    Write_Ds1302_Byte (0x80, 0x30);
    Write_Ds1302_Byte (0x82, 0x59);
    Write_Ds1302_Byte (0x84, 0x23);
    Write_Ds1302_Byte (0x86, 1);
    Write_Ds1302_Byte (0x88, 1);
    Write_Ds1302_Byte (0x8a, 1);
    Write_Ds1302_Byte (0x8c, 0);
    Write_Ds1302_Byte (0x8e, 0x80);
}

// 十进制转BCD码
u8 Dec2Bcd (u8 src) {
    return ((src / 10) << 4) | (src % 10);
}

// BCD码转十进制
u8 Bcd2Dec (u8 src) {
    return ((src >> 4) * 10 + (src & 0x0f));
}

// 读取当前时间
void DS1302_Read_Current_Time() {
    currentTime.hh = Bcd2Dec (Read_Ds1302_Byte (0x85));
    currentTime.mm = Bcd2Dec (Read_Ds1302_Byte (0x83));
    currentTime.ss = Bcd2Dec (Read_Ds1302_Byte (0x81));
}

// 写入指定时间，但没用到
void DS1302_Write_Time (TIME *src) {
    Write_Ds1302_Byte (0x8e, 0);
    Write_Ds1302_Byte (0x80, Dec2Bcd (src->ss));
    Write_Ds1302_Byte (0x82, Dec2Bcd (src->mm));
    Write_Ds1302_Byte (0x84, Dec2Bcd (src->hh));
    Write_Ds1302_Byte (0x8e, 0x80);
}
