#include "pcf.h"

u8 last_Voltage = 52;

void PCF_Voltage_Out(u8 vol) {
    IIC_Start();
    IIC_SendByte(0x90);
    IIC_WaitAck();
    IIC_SendByte(0x40);
    IIC_WaitAck();
    IIC_SendByte(vol);
    IIC_WaitAck();
    IIC_Stop();
    last_Voltage = vol;
}

u8 PCF_Channel_Measure(u8 ch) {
    u16 result;
    IIC_Start();
    IIC_SendByte(0x90);
    IIC_WaitAck();
    IIC_SendByte(0x40 | ch);
    IIC_WaitAck();
    IIC_SendByte(last_Voltage);
    IIC_WaitAck();
    IIC_Stop();

    IIC_Start();
    IIC_SendByte(0x91);
    IIC_WaitAck();
    result = IIC_RecByte();
    IIC_SendAck(0); 
    result += IIC_RecByte();
    IIC_Stop();
    return (u8)(result / 2);
}
