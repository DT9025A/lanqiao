#include "ds18b20.h"

void Delay100us()		//@12.000MHz
{
	unsigned char i, j;

	i = 2;
	j = 39;
	do
	{
		while (--j);
	} while (--i);
}

float DS18B20_ReadTemp() {
    unsigned int result = 0;

    //EA = 0;
    if(Init_DS18B20() == 1)
        return 255.0f;
    Write_DS18B20(0xcc);
    Write_DS18B20(0x44);
    Delay100us();
    if(Init_DS18B20() == 1)
        return 255.0f;
    Write_DS18B20(0xcc);
    Write_DS18B20(0xbe);
    result = Read_DS18B20();
    result |= (Read_DS18B20() << 8);
    //EA = 1;
    return result * 0.0625f;
}