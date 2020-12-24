#include "ff.h"

double TC_Read()
{
	uint16_t data;
	T_CS=0;

	data += GPIO_ReadBit(HW_GPIOA, 28);
	data<<=1;
	
	for(uint8_t i=0;i<15;i++)
	{
		T_SCK=1;
		
		data += GPIO_ReadBit(HW_GPIOA, 28);
		data<<=1;

		T_SCK=0;
	}
	T_CS=1;
	
//	SC_1_R_W=1;
//	
//	printf(",data:0x%04X,",data);
//	
//	DelayMs(2);
//	SC_1_R_W=0;
	
	data>>=3;
	data&=0xFFF;
	

	
	double tc;
	tc=data;
	tc*=0.25;
	
	return tc;
}




















