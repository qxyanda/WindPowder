#include "ff.h"

void X9C(uint8_t x)
{
	switch (x)
	{
	
			case 0: 
		{
			GPIO_WriteBit(HW_GPIOA,16, 0);
			SYSTICK_DelayUs(10);
			GPIO_WriteBit(HW_GPIOA,13, 1);
			SYSTICK_DelayUs(10);
			GPIO_WriteBit(HW_GPIOA,15, 1);
			
			SYSTICK_DelayUs(50);
			GPIO_WriteBit(HW_GPIOA,13, 0);
			GPIO_WriteBit(HW_GPIOA,15, 0);
			
			SYSTICK_DelayUs(50);			
			GPIO_WriteBit(HW_GPIOA,15, 1);
			SYSTICK_DelayUs(10);
			GPIO_WriteBit(HW_GPIOA,13, 1);
			
			SYSTICK_DelayUs(50);
			break;
		}
		
				case 1: 
		{
			GPIO_WriteBit(HW_GPIOA,16, 1);
			SYSTICK_DelayUs(10);
			GPIO_WriteBit(HW_GPIOA,13, 1);
			SYSTICK_DelayUs(10);
			GPIO_WriteBit(HW_GPIOA,15, 1);
			
			SYSTICK_DelayUs(50);
			GPIO_WriteBit(HW_GPIOA,13, 0);
			GPIO_WriteBit(HW_GPIOA,15, 0);
			
			SYSTICK_DelayUs(50);			
			GPIO_WriteBit(HW_GPIOA,15, 1);
			SYSTICK_DelayUs(10);
			GPIO_WriteBit(HW_GPIOA,13, 1);
			
			SYSTICK_DelayUs(50);
			break;
		}
		
				case 9: 
		{

			GPIO_WriteBit(HW_GPIOA,13, 0);
			SYSTICK_DelayUs(10);
			GPIO_WriteBit(HW_GPIOA,15, 0);
			
			SYSTICK_DelayUs(50);
			GPIO_WriteBit(HW_GPIOA,13, 1);
			
			SYSTICK_DelayUs(50);
			break;
		}
	}
	
}
