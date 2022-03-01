#include "pcf.h"

void PCF_Config(u8 mode, u8 dac) {
    IIC_Start();
    IIC_SendByte(0x90);
    IIC_WaitAck();
    IIC_SendByte(mode);
    IIC_WaitAck();
    IIC_SendByte(dac);
    IIC_WaitAck();
    IIC_Stop();
}


u8 PCF_Read() {
    u8 result = 255;
    IIC_Start();
    IIC_SendByte(0x91);
    IIC_WaitAck();
    result = IIC_RecByte();
    IIC_SendAck(1);
    IIC_Stop();
    return result;
}
