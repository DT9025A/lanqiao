#include "at24.h"

void AT24_Write(u8 addr, u8 dat) {
    IIC_Start();
    IIC_SendByte(0xa0);
    IIC_WaitAck();
    IIC_SendByte(addr);
    IIC_WaitAck();
    IIC_SendByte(dat);
    IIC_WaitAck();
    IIC_Stop();
}

u8 AT24_Read(u8 addr) {
    u8 dat;

    IIC_Start();
    IIC_SendByte(0xa0);
    IIC_WaitAck();
    IIC_SendByte(addr);
    IIC_WaitAck();
    
    IIC_Start();
    IIC_SendByte(0xa1);
    IIC_WaitAck();
    dat = IIC_RecByte();
    IIC_SendAck(1);
    IIC_Stop();
    return dat;
}
