#include "onewire.h"

void Delay60us()		//@12.000MHz
{
	unsigned char i, j;

	i = 1;
	j = 176;
	do
	{
		while (--j);
	} while (--i);
}

void Delay40us()		//@12.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 117;
	while (--i);
}

void Delay10us()		//@12.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 27;
	while (--i);
}


//单总线写操作
void Write_DS18B20(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		DQ = 0;
		DQ = dat&0x01;
		Delay40us();
		DQ = 1;
		dat >>= 1;
		Delay40us();
	}
	Delay40us();
}

//单总线读操作
unsigned char Read_DS18B20(void)
{
	unsigned char i;
	unsigned char dat;
  
	for(i=0;i<8;i++)
	{
		DQ = 0;
		dat >>= 1;
        Delay10us();
		DQ = 1;
		if(DQ)
		{
			dat |= 0x80;
		}	    
		Delay40us();
	}
	return dat;
}

//DS18B20初始化
bit init_ds18b20(void)
{
  	bit initflag = 0;
  	
  	DQ = 1;
  	Delay60us();
  	DQ = 0;
  	Delay60us();
  	Delay60us();
  	Delay60us();
  	Delay60us();
  	Delay60us();
  	Delay60us();
  	Delay60us();
  	Delay60us();
  	Delay60us();
  	DQ = 1;
  	Delay60us();
  	Delay60us();
    initflag = DQ;
  	Delay60us();    
  	Delay60us();
  	Delay60us(); 
  
  	return initflag;
}

u8 rd_temperature(void) {
    u16 tmp;
    if(init_ds18b20() == 1)
        return 0xff;
    Write_DS18B20(0xcc);
    Write_DS18B20(0x44);
  	Delay60us();
    if(init_ds18b20() == 1)
        return 0xff;
    Write_DS18B20(0xcc);
    Write_DS18B20(0xbe);
    tmp = Read_DS18B20();
    tmp += Read_DS18B20() << 8;
    tmp &= 0x7ff;
    return tmp >> 4 ; 
} 
