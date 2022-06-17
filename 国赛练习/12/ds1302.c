#include "ds1302.h"

TIME read_time;

//
void Write_Ds1302 (unsigned  char temp) {
    unsigned char i;
    for (i = 0; i < 8; i++) {
        SCK = 0;
        SDA = temp & 0x01;
        temp >>= 1;
        SCK = 1;
    }
}

//
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

//
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


void DS1302_Init() {
    Write_Ds1302_Byte (0x8e, 0);
    Write_Ds1302_Byte (0x80, 0x00);
    Write_Ds1302_Byte (0x82, 0x20);
    Write_Ds1302_Byte (0x84, 0x20);
    Write_Ds1302_Byte (0x8e, 0x80);
}

u8 BCD2HEX (u8 bcd) {
    return ((bcd >> 4) * 10 + (bcd & 0x0f));
}

void DS1302_Read() {
    read_time.ss = BCD2HEX (Read_Ds1302_Byte (0x81));
    read_time.mm = BCD2HEX (Read_Ds1302_Byte (0x83));
    read_time.hh = BCD2HEX (Read_Ds1302_Byte (0x85));
}
