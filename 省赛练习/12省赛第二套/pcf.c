#include "pcf.h"

void PCF8591_Init (unsigned char channel) {
    IIC_Start();
    IIC_SendByte (0x90);
    IIC_WaitAck();
    IIC_SendByte (channel);
    IIC_WaitAck();
    IIC_SendByte (0);
    IIC_WaitAck();
    IIC_Stop();
    PCF8591_Read();
}

unsigned char PCF8591_Read() {
    unsigned char tmp;
    IIC_Start();
    IIC_SendByte (0x91);
    IIC_WaitAck();
    tmp = IIC_RecByte(); 
    IIC_SendAck(1); 
    IIC_Stop();
    return tmp;
}