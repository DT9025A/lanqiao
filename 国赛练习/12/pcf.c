#include "pcf.h"

u8 lastVoltageOut;

void PCF_Out_Voltage(u8 v) {
    IIC_Start();
    IIC_SendByte(0x90);
    IIC_WaitAck();
    IIC_SendByte(0x40);
    IIC_WaitAck();
    IIC_SendByte(v);
    IIC_WaitAck();  
    IIC_Stop(); 
    lastVoltageOut = v;
}

u8 PCF_Read_Channel(u8 ch) {
    u8 dat;
    IIC_Start();
    IIC_SendByte(0x90);
    IIC_WaitAck();
    IIC_SendByte(0x40 | ch);
    IIC_WaitAck();
    IIC_SendByte(lastVoltageOut);
    IIC_WaitAck();  
    IIC_Stop(); 

    IIC_Start();
    IIC_SendByte(0x91);
    IIC_WaitAck();
    dat = IIC_RecByte();
    IIC_SendAck(0);
    dat = IIC_RecByte();
    IIC_SendAck(1);  
    IIC_Stop(); 
    return dat;
}