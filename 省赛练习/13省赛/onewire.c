#include "onewire.h"

void Delay10us() {	//@12.000MHz
    unsigned char i;

    _nop_();
    _nop_();
    i = 27;
    while (--i);
}


void Delay40us() {	//@12.000MHz
    unsigned char i;

    _nop_();
    _nop_();
    i = 117;
    while (--i);
}

void Delay500us() {	//@12.000MHz
    unsigned char i, j;

    i = 6;
    j = 211;
    do {
        while (--j);
    } while (--i);
}

void Write_DS18B20 (unsigned char dat) {
    unsigned char i;
    for (i = 0; i < 8; i++) {
        DQ = 0;
        Delay10us();
        DQ = dat & 0x01;
        Delay10us();
        Delay40us();
        DQ = 1;
        dat >>= 1;
    }
    Delay40us();
}


unsigned char Read_DS18B20 (void) {
    unsigned char i;
    unsigned char dat;

    for (i = 0; i < 8; i++) {
        DQ = 0;
        dat >>= 1;
        Delay10us();
        DQ = 1;
        if (DQ) {
            dat |= 0x80;
        }
        Delay40us();
    }
    return dat;
}


bit init_ds18b20 (void) {
    bit initflag = 0;

    DQ = 1;
    Delay40us(); //PULL HIGH
    DQ = 0;
    Delay500us(); //480µs minimum
    DQ = 1;
    Delay40us();
    Delay40us(); //DS18B20 waits 15-60µs
    initflag = DQ;
    Delay40us();
    Delay40us(); //presence pulse 60-240µs

    return initflag;
}

float rd_temperature (void) {
    u16 dat;
    if (init_ds18b20())
        return -255.0;
    // 跳过ROM匹配
    Write_DS18B20 (0xcc);
    // 转换温度
    Write_DS18B20 (0x44);
    Delay500us();
    Delay500us();
    if (init_ds18b20())
        return -255.0;
    // 跳过ROM匹配
    Write_DS18B20 (0xcc);
    // 读取SCRATCHPAD
    Write_DS18B20 (0xbe);
    dat = Read_DS18B20();
    dat |= Read_DS18B20() << 8;
    // 最低位为2^-4
    return (float)dat * 0.0625;
}
