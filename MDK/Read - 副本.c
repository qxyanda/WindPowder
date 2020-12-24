#include "gpio.h"
#include "common.h"
#include "uart.h"
#include "adc.h"
#include "wdog.h"
#include "systick.h"
#include "arm_math.h"
#include "MK64F12.h"
#include <stdint.h>

#include "stdio.h"
#include "includes64.h"
#include "LQ12864.h"
#include "gpio64.h"
#include "gpio.h"
#include "pit.h"

#define AD_0 ADC0_SE3_DP3 
#define AD_1 ADC1_SE3_DP3

#define delta 70


#define dotnum 2048

extern uint32_t count,N_start,time,mit;
extern float32_t data0[dotnum],data1[dotnum],pOut0[dotnum],pOut1[dotnum];
extern float32_t pMin,pMax,trash;



void ADCvt()
{
			count=0;
			for(int16_t i=0;i<dotnum+N_start;i++)
			{
				if(i<dotnum)
				{
					data0[i]=ADC_QuickReadValue(AD_0);
//					data0[i]/=65535;
				}
				else
				{
					trash=ADC_QuickReadValue(AD_0);
//					trash/=65535;
				}
				if(i>=N_start)//
				{
					data1[i-N_start]=ADC_QuickReadValue(AD_1);
//					data1[i-N_start]/=65535;
				}
				else
				{
					trash=ADC_QuickReadValue(AD_1);
//					trash/=65535;
				}
				
//				if(i%2==0){
//				data0[i]=0;}
//				if((i-N_start)%2==0){
//				data0[i-N_start]=0;}
				
//				SYSTICK_DelayUs(delta);
			}		
			time=count;


////////////////////////////归一化//////////////////////////////
			
			arm_min_f32(data0,dotnum,&pMin,&mit);			//min
			pMin=-pMin;																//-min
			arm_offset_f32(data0,pMin,pOut0,dotnum);	//x-min
			
			arm_max_f32(pOut0,dotnum,&pMax,&mit);			//max-min
			pMax=1/pMax;															//1/(max-min)
			arm_scale_f32(pOut0,pMax,data0,dotnum);		//(x-min)/(max-min)
			
			arm_min_f32(data1,dotnum,&pMin,&mit);
			pMin=-pMin;
			arm_offset_f32(data1,pMin,pOut1,dotnum);

			arm_max_f32(pOut1,dotnum,&pMax,&mit);
			pMax=1/pMax;
			arm_scale_f32(pOut1,pMax,data1,dotnum);
			
/////////////////////////////归一化//////////////////////////////	

//			  arm_max_f32(data0,dotnum,&mmm,&pIndex);
//				arm_min_f32(data0,dotnum,&iii,&pIndex);
			
}
