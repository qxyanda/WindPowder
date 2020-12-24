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

#define SIM_SCGC4_LLWU_MASK   0x10000000u
#define dotnum 2048

/*
静电
2016.07.18.阎达
*/

//static uint8_t kono_addr=0x01;//本机地址

#define CS HW_GPIOE^4
#define SO HW_GPIOE^2
#define SCK HW_GPIOE^0


#define SC_0 UART3_RX_PB10_TX_PB11  //UART3  UART3
#define SC_1 UART0_RX_PB16_TX_PB17  //UART0  UART0
#define SC_2 UART1_RX_PC03_TX_PC04

#define AD_0 ADC0_SE3_DP3 
#define AD_1 ADC1_SE3_DP3
#define AD_2 ADC1_SE1_DP1 
#define AD_3 ADC0_SE1_DP1


static void UART_RX_ISR0(uint16_t byteReceived);//主MCU串口中断子程序
static void UART_RX_ISR3(uint16_t byteReceived);


//锁相环频率为50/15*54=180M测试函数
extern void pllinit100M(void);
extern void pllinit180M(void);

extern void ADCvt(void);

void Disp(void);

//float32_t T1_Read(void);
//float32_t T2_Read(void);

//static float32_t VREFH = 3.0;
float32_t pResult=0,
					mean=0,
//					Q[10]={0,0,0,0,0,0,0,0,0,0},	
					Qmax=0,
					Q=0,
					pMax=0,
					pMin=0,
					v_pre=0,
					zeit_pre=0,
					trash=0,
					velocity=0,
					zeit=0,
					diff=0;
					
float32_t p[dotnum],
					q[dotnum],
					
					pOut0[dotnum],//FFT1
					pOut1[dotnum];//FFT2


extern float32_t data0[dotnum],
								 data1[dotnum];


uint8_t error=0,
				cycle=1;

					
uint32_t 	pIndex=0,
					count=0,
					time=0,
					N_start=0,
					null=0,
					mit=0;
				 


//float32_t mmm=0,iii=0 ;
//标定

//static float32_t T = 0;


void PIT_ISR(void)
{
    count++;
}



int main(void)
{
//    GPIO_QuickInit(HW_GPIOE, 0, kGPIO_Mode_OPP);	

	
	  pllinit180M();
	
    SYSTICK_DelayInit();

	  UART_QuickInit(SC_0, 115200);	//9600,N,8,1
	  UART_QuickInit(SC_1, 115200);	//9600,N,8,1

	
	  /*  串口中断 */
    UART_CallbackRxInstall(HW_UART0, UART_RX_ISR0);
    /* 打开串口接收中断功能 */
    UART_ITDMAConfig(HW_UART0, kUART_IT_Rx, true);
	
	  UART_CallbackRxInstall(HW_UART3, UART_RX_ISR3);
    /* 打开串口接收中断功能 */
    UART_ITDMAConfig(HW_UART3, kUART_IT_Rx, true);
	
		UART_SelectDebugInstance(HW_UART3);	

		WDOG_QuickInit(5000);

		LCD_Init();	

			
		ADC_QuickInit(AD_0, kADC_SingleDIff16);
		ADC_QuickInit(AD_1, kADC_SingleDIff16);
		
			    /* 初始化PIT模块 */
    PIT_InitTypeDef PIT_InitStruct1;  //申请结构体变量
    PIT_InitStruct1.chl = HW_PIT_CH0; /* 使用0号定时器 */
    PIT_InitStruct1.timeInUs = 1; /* 定时周期1S *///us
    PIT_Init(&PIT_InitStruct1); //pit模块初始化
    /* 注册PIT 中断回调函数 */
    PIT_CallbackInstall(HW_PIT_CH0, PIT_ISR); //0号定时器的中断处理
    /* 开启PIT0定时器中断 */
    PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, true);

		
main_loop:for(;;)//main loop
    {
			N_start=0;
			error=0;
//			for(uint8_t i=0;i<10;i++)
//			{
//				Q[i]=0;
//			}
			Q=0;
			
//				for(uint8_t j=0;j<cycle;j++)
//			{
			
				count=0;
				ADCvt();


			

			
				arm_rfft_fast_instance_f32 S;
				arm_rfft_fast_init_f32(&S, dotnum);
			
				arm_rfft_fast_f32(&S,data1,pOut0,0);//FFT1
				arm_rfft_fast_f32(&S,data0,pOut1,0);//FFT2	
	

				arm_cmplx_conj_f32(pOut1,data0,dotnum>>1);//CONJ  arm_cmplx_conj_f32(pOut1,pDst1,dotnum>>1);//CONJ

				arm_cmplx_mult_cmplx_f32(pOut0,data0,data1,dotnum>>1);//MULT_CONJ  arm_cmplx_mult_cmplx_f32(pOut0,pDst1,pDst0,dotnum>>1);//MULT_CONJ
	
				arm_rfft_fast_f32(&S,data1,data0,1);//RFFT  arm_rfft_fast_f32(&S,pDst0,Res,1);//RFFT

				arm_max_f32(data0,dotnum/2,&pResult,&pIndex);//MAX  arm_max_f32(Res,dotnum,&pResult,&pIndex);//MAX
				
				arm_mean_f32(data0,dotnum,&mean);

//Q>=1.06&&<=1.3638=>相关

//				Q[j]=pResult;
//				Q[j]/=mean;
	
	
//				Q = pResult + 0.01;
//				Q /= (mean + 0.01);
//				
//				Qmax = (Q > Qmax)? Q : Qmax;
				
				
//				if(0){break;}//Q[j]>(float32_t)3

//Q>=1.06&&<=1.3638=>相关
//				else
//				{
//					N_start+=(j!=cycle)?dotnum:0;
//				}
//			}
			
//			arm_max_f32(Q,10,&pResult,&mit);
//			Qmax=pResult;
			zeit = pIndex;//dotnum + N_start - 
//			zeit += (N_start * mit);
			zeit *= time;
			zeit /= (dotnum + 1) ;// + N_start
			zeit /= 1000;
			

			
			null=0;			
			
//		if(Q<=(float32_t)Qmax*0.8)//
//		{
//			error=1;
//		}
//		else
//		{
//		
//			if(zeit<=10)
//			{
//				error=1;
//				N_start=0;
//			}
//			else
//			{
//				if(zeit>=100)
//				{	
//					null=1;
//					N_start=0;
//				}
//				else
//				{
//					error=0;
//					null=0;
//				}
//			}
//		}	
		
		if(0)//!error&&!null
		{
			zeit+=zeit_pre;
			zeit/=2;
			zeit_pre=zeit;

		

			
			
//偏移
//			if(pIndex<(dotnum>>1))//
//			{
//				N_start+=(dotnum>>1);
//				N_start-=pIndex;
//			}
//			else
//			{
//				N_start=0;
//			}
//			
//			if(zeit > 500)
//			{
//				N_start = 0;
//			}
//偏移				
					}
			
			velocity=(zeit>0.06)?120/zeit:0;

			diff=(v_pre>0.01)?velocity/v_pre-1:0.1;
					
if(diff<0)
{
	diff=-diff;
}

if(diff<0.2)
{
	velocity += v_pre * 4 ;
	velocity /=5;
	v_pre = velocity;
}
else
{
	continue;
}
		


//			LCD_CLS();                          //清屏	
		
			
			Q=0;	
			Disp();
			WDOG_Refresh(); //黑喂狗
}			

}


void Disp()
{


			char txt[10];
			sprintf(txt, "Q %5.3f", Q);
			LCD_P8x16Str(0,2,txt);
		
			sprintf(txt, "%6dus", time);
//					sprintf(txt, "%6.2f", mmm);
			LCD_P8x16Str(0,4,txt);
			


			sprintf(txt, "%9.2fms", zeit);
			LCD_P8x16Str(0,0,txt);
	
			sprintf(txt, "%6d", pIndex);
			LCD_P8x16Str(64,2,txt);	

			sprintf(txt, "%9.2fm/s", velocity);
			LCD_P8x16Str(0,6,txt);


			sprintf(txt, "%6dus", count);
	
//	  sprintf(txt, "%6.2f", iii);
	
			LCD_P8x16Str(64,4,txt);

//			GPIO_ToggleBit(HW_GPIOE, 0);


}



void UART_RX_ISR0(uint16_t byteReceived)
{

}

void UART_RX_ISR3(uint16_t byteReceived)
{
	
}





