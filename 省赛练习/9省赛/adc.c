#include "adc.h"

void InitADC() {
    IIC_Start();
    IIC_SendByte (0x90);
    if (IIC_WaitAck())
        return;
    IIC_SendByte (3);
    if (IIC_WaitAck())
        return;
    IIC_SendByte (0);
    if (IIC_WaitAck())
        return;
    IIC_Stop();
}

unsigned char ReadADCLevel() {
    unsigned char tmp;
    IIC_Start();
    IIC_SendByte(0x91);
    if (IIC_WaitAck())
		return 0;
    tmp = IIC_RecByte();
    IIC_SendAck(1);
    IIC_Stop();
    return tmp;
}
