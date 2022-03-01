#include "at24c02.h"

void WriteEEPROM(unsigned char add, unsigned char dat)
{
    IIC_Start();
    IIC_SendByte(0xA0);
    if (IIC_WaitAck())
		return;
    IIC_SendByte(add);
    if (IIC_WaitAck())
		return;
    IIC_SendByte(dat);
    if (IIC_WaitAck())
		return;
    IIC_Stop();
}

unsigned char ReadEEPROM(unsigned char add)
{
    unsigned char tmp = 255;
    IIC_Start();
    IIC_SendByte(0xA0);
    if (IIC_WaitAck())
		return 255;
    IIC_SendByte(add);
    if (IIC_WaitAck())
		return 255;
    IIC_Start();
    IIC_SendByte(0xA1);
    if (IIC_WaitAck())
		return 255;
    tmp = IIC_RecByte();
    IIC_SendAck(1);
    IIC_Stop();
    return tmp;
}
