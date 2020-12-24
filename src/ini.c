#include "ff.h"



extern float32_t alpha_0;
//void UART_RX_ISR3(uint16_t byteReceived)
//{

//}
extern void X9C103_Init(unsigned char Sel);//初始化至中间位置
extern void X9C103_Inc_N_Step(unsigned char Sel,unsigned char N);
extern void X9C103_Dec_N_Step(unsigned char Sel,unsigned char N);

extern uint8_t call;
extern uint32_t NU_sum;
extern float32_t v_pre;
uint8_t buf[PAGE_SIZE];

uint8_t i=0,
				lock=1,
				full=0;
uint8_t ch[17];


uint8_t ch_HostRe[8];//single
uint8_t ch_HostOutput[9];//single

extern float32_t alpha_vector[8];
extern float32_t v_lv;
uint8_t addr_slave=0x00;

//////////////////
//host addr 0*01//
uint8_t addr_host=0x01;
//host addr 0*01//
//////////////////

uint8_t addr_c=0x01;

uint8_t Re=0;

uint8_t isDebugMode=0;

void addr_read(void);

extern uint8_t echo_data[4];

extern void comdata_handle(void);


uint16_t set[10]={0,0x1388,0x02,4096,10,100,0,0,0,0};


extern uint16_t force;
extern void echo(void);

void BusIni(void);
void BusSet(bool out);

extern uint8_t iniCount;
uint8_t outputEn=0;
uint8_t ini()
{

	/* DELAY INI */	
	DelayInit();
  SYSTICK_DelayInit();
	/* DELAY INI */	
	
	/* WDG INI */	
	WDOG_QuickInit(2000);
	/* WDG INI */	

	/* ADC SPI INI */
	GPIO_QuickInit(HW_GPIOA, 19, kGPIO_Mode_OPP);//ADDR
	
	GPIO_QuickInit(HW_GPIOA, 24, kGPIO_Mode_IFT);//DA
	GPIO_QuickInit(HW_GPIOA, 25, kGPIO_Mode_IFT);//DB
	GPIO_QuickInit(HW_GPIOA, 26, kGPIO_Mode_IFT);//BUSY
	
	GPIO_QuickInit(HW_GPIOA, 27, kGPIO_Mode_OPP);//CNVST
	GPIO_QuickInit(HW_GPIOA, 28, kGPIO_Mode_OPP);//SCLK
	GPIO_QuickInit(HW_GPIOA, 29, kGPIO_Mode_OPP);//CS
	
	GPIO_QuickInit(HW_GPIOA, 16, kGPIO_Mode_OPP);//RANGE0
	GPIO_QuickInit(HW_GPIOA, 17, kGPIO_Mode_OPP);//RANGE1
	/* ADC SPI INI */
	
	//ADC reset
	RANGE0=1;
	RANGE1=1;
	ADDR=1;
	//ADC reset

//	CS=1;

	DelayMs(1);
	
	BusIni();
	
	return 0;
}

void BusIni()
{
	BusSet(true);
	
	switch(slaveAddr)
	{
		case 0:{
			GPIO_QuickInit(HW_GPIOD, 0, kGPIO_Mode_OPP);
			GPIO_QuickInit(HW_GPIOD, 4, kGPIO_Mode_IFT);
			GPIO_QuickInit(HW_GPIOD, 5, kGPIO_Mode_IFT);
			
				GPIO_QuickInit(HW_GPIOC, 0, kGPIO_Mode_IFT);//15
	GPIO_QuickInit(HW_GPIOC, 1, kGPIO_Mode_IFT);//14
	GPIO_QuickInit(HW_GPIOC, 2, kGPIO_Mode_OPP);//13
			break;
		}
		case 1:{
			GPIO_QuickInit(HW_GPIOD, 4, kGPIO_Mode_OPP);
			GPIO_QuickInit(HW_GPIOD, 0, kGPIO_Mode_IFT);
			GPIO_QuickInit(HW_GPIOD, 5, kGPIO_Mode_IFT);
			
				
	GPIO_QuickInit(HW_GPIOC, 3, kGPIO_Mode_IFT);//12
	GPIO_QuickInit(HW_GPIOC, 4, kGPIO_Mode_IFT);//11
	GPIO_QuickInit(HW_GPIOC, 5, kGPIO_Mode_OPP);//10
			break;
		}
		case 2:{
			GPIO_QuickInit(HW_GPIOD, 5, kGPIO_Mode_OPP);
			GPIO_QuickInit(HW_GPIOD, 0, kGPIO_Mode_IFT);
			GPIO_QuickInit(HW_GPIOD, 4, kGPIO_Mode_IFT);
			
				
	GPIO_QuickInit(HW_GPIOC, 6, kGPIO_Mode_IFT);//9
	GPIO_QuickInit(HW_GPIOC, 7, kGPIO_Mode_IFT);//8
	GPIO_QuickInit(HW_GPIOC, 8, kGPIO_Mode_OPP);//7
		break;
		}
	}
	GPIO_QuickInit(HW_GPIOC, 19, kGPIO_Mode_IFT);
	GPIO_QuickInit(HW_GPIOD, 1, kGPIO_Mode_IFT);
	GPIO_QuickInit(HW_GPIOD, 2, kGPIO_Mode_IFT);
	GPIO_QuickInit(HW_GPIOD, 3, kGPIO_Mode_IFT);
}

void BusSet(bool out)
{
//	if(out)
//	{
//		for(uint8_t i=0;i<16;i++)
//		GPIO_QuickInit(HW_GPIOC, i, kGPIO_Mode_OPP);
//	}
//	else
//	{
//		for(uint8_t i=0;i<16;i++)
//		GPIO_QuickInit(HW_GPIOC, i, kGPIO_Mode_IFT);
//	}
	




}
