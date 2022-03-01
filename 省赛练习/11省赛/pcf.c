#include "pcf.h"

void PCF_Init() {
    IIC_Start();
    IIC_SendByte (0x90);
    IIC_WaitAck();
    IIC_SendByte (3);
    IIC_WaitAck();
    IIC_Stop();
}

u8 PCF_Read() {
    u8 r;
    IIC_Start();
    IIC_SendByte (0x91);
    IIC_WaitAck();
    r = IIC_RecByte();
    IIC_SendAck (1);
    IIC_Stop();
    return r;
}