#include "pcf8591.h"

void PCF8591_Config(unsigned char ctrl, unsigned char dac){
    IIC_Start();
    IIC_SendByte(0x90);
    if(IIC_WaitAck())
        return;
    IIC_SendByte(ctrl);
    if(IIC_WaitAck())
        return;
    IIC_SendByte(dac);
    if(IIC_WaitAck())
        return;
    IIC_Stop();
}

unsigned char PCF8591_Read() {
    unsigned char tmp;
    IIC_Start();
    IIC_SendByte(0x91);
    if(IIC_WaitAck())
        return 255;
    tmp = IIC_RecByte();
    IIC_SendAck(1);
    IIC_Stop();
    return tmp;
}
