#include "ds18b20.h"

void Delay120ms()		//@12.000MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 6;
	j = 121;
	k = 189;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
                      
unsigned char ReadTemp() {
    unsigned int tmp = 0;
    
    init_ds18b20();
    Write_DS18B20(0xcc);
    Write_DS18B20(0x44);
    Delay120ms();
    init_ds18b20();
    Write_DS18B20(0xcc);
    Write_DS18B20(0xbe);
    tmp = Read_DS18B20();
    tmp |= (Read_DS18B20() << 8);
    tmp &= 0x7ff;
    tmp >>= 4;

    return (unsigned char) tmp;
}
