#include "ff.h"

extern float32_t 
					conZen,
					v_lv;

extern uint8_t addr_slave;
extern uint8_t addr_host;
extern uint8_t isDebugMode;

extern uint8_t ch_HostRe[8];//single
extern uint8_t ch_HostOutput[9];//single

extern uint8_t echo_data[4];


extern float32_t G;
extern float32_t a;
extern float32_t b;
extern float32_t std;

//host

void data_store(void);
void host_output(void);
void Bus(void);
void DataTrans(float32_t data,uint16_t scale);
void SPI(void);
	
float32_t vVecNow;
float32_t cVecNow;
	
extern uint8_t iniCount;
void data_store(){
	
//	if((uint16_t)(a*(conZen-b)/G)<100)v_lv=0;
	//数据格式需要修改2019.10.31 int16(x*100)
//	while(v_lv>25)v_lv*=0.8;
//	while(v_lv<15&&v_lv!=0)v_lv*=1.5;
	//已修改
	uint16_t v_out=0;
//	if(std<100)vVecNow=0;
	//风速

	v_out=(uint16_t)(vVecNow*100);
	

	
//	if(isDebugMode)v_out=addr_slave*100;
	//HI
	echo_data[0]=(v_out>>8) & 0x00ff;
	//LO
	echo_data[1]=v_out & 0x00ff;
	
	//浓度 100.00%
	v_out=(uint16_t)(a*(cVecNow-b)/G)*4*0.5;
	

//	if(v_out>10000)v_out=10000;
//	while(v_out<1000 && (uint16_t)(a*(conZen-b)/G)>50)v_out*=1.8;//3000/50
	
	if(v_out<=0||v_out>10000)v_out=2550;
	
//	if(isDebugMode)v_out=addr_slave*100;
//	v_out=conZen;
	
	//HI
	echo_data[2]=(v_out>>8) & 0x00ff;
	//LO
	echo_data[3]=v_out & 0x00ff;
}


void host_output()
{
	//输出数据赋值
	//主机地址[1][1][1]
	ch_HostOutput[0]=addr_slave;
	ch_HostOutput[1]=0x03;
	ch_HostOutput[2]=0x04;
	//风速浓度数据[4]
	for(uint8_t i=0;i<4;i++)
	ch_HostOutput[i+3]=echo_data[i];
	//计算crc校验
  uint16_t result = CRC16_GenerateSoftware(ch_HostOutput, 7);
	
	//crc[2]
	//低8位
	ch_HostOutput[7]=result & 0x00ff;
	//高8位
	ch_HostOutput[8]=(result>>8) & 0x00ff;
	
	SC_0_R_W=1;
	DelayMs(1);
	
	//输出数据到7188[9]
	for(uint8_t i=0;i<9;i++)
	{
		UART_WriteByte(HW_UART5, ch_HostOutput[i]);
	}

	DelayMs(2);
	SC_0_R_W=0;
	
}


extern float32_t Q;
extern void BusSet(bool out);
void Bus()
{
	switch(slaveAddr)
	{
		case 0:{
			FB_B0=1;
			DelayUs(500);
			
			while(FB_CS0!=0 || FB_CS1!=0);
			DelayUs(500);
			while(FB_CS0!=0 || FB_CS1!=0);
			
			
			BusSet(true);//bus输出
			DelayUs(500);
			
			while(FB_A0!=0 || FB_A1!=0);
			DelayUs(500);
			while(FB_A0!=0 || FB_A1!=0);
			FB_B0=0;
			DataTrans(v_lv,100);
			FB_B0=1;
			
			while(FB_A0!=1 || FB_A1!=0);
			DelayUs(500);
			while(FB_A0!=1 || FB_A1!=0);
			
			FB_B0=0;
			DataTrans(conZen,1);
			FB_B0=1;
			
			while(FB_A0!=0 || FB_A1!=1);
			DelayUs(500);
			while(FB_A0!=0 || FB_A1!=1);
			
			FB_B0=0;
			DataTrans(Q,10000);
			FB_B0=1;
			
			while(FB_CS0==0 && FB_CS1==0);
			DelayUs(500);
			while(FB_CS0==0 && FB_CS1==0);
			BusSet(false);//bus悬空
			FB_B0=0;
			
			break;
		}
		case 1:{
			FB_B1=1;
			DelayUs(100);
			
			while(FB_CS0!=1 || FB_CS1!=0);
			DelayUs(100);
			while(FB_CS0!=1 || FB_CS1!=0);
			
			BusSet(true);//bus输出
			DelayUs(100);
			
			while(FB_A0!=0 || FB_A1!=0);
			DelayUs(100);
			while(FB_A0!=0 || FB_A1!=0);
			
			DataTrans(v_lv,100);
			while(FB_A0!=1 || FB_A1!=0);
			DelayUs(100);
			while(FB_A0!=1 || FB_A1!=0);
			
			DataTrans(conZen,1);
			while(FB_A0!=0 || FB_A1!=1);
			DelayUs(100);
			while(FB_A0!=0 || FB_A1!=1);
			DataTrans(Q,10000);
			
			
			while(FB_CS0==1 && FB_CS1==0);
			DelayUs(100);
			while(FB_CS0==1 && FB_CS1==0);
			BusSet(false);//bus悬空
			FB_B1=0;
			
			break;
		}
		case 2:{
			FB_B2=1;
			DelayUs(100);
			
			while(FB_CS0!=0 || FB_CS1!=1);
			DelayUs(100);
			while(FB_CS0!=0 || FB_CS1!=1);
			
			BusSet(true);//bus输出
			DelayUs(100);
			
			while(FB_A0!=0 || FB_A1!=0);
			DelayUs(100);
			while(FB_A0!=0 || FB_A1!=0);
			
			DataTrans(v_lv,100);
			while(FB_A0!=1 || FB_A1!=0);
			DelayUs(100);
			while(FB_A0!=1 || FB_A1!=0);
			
			DataTrans(conZen,1);
			while(FB_A0!=0 || FB_A1!=1);
			DelayUs(100);
			while(FB_A0!=0 || FB_A1!=1);
			
			DataTrans(Q,10000);
			
			
			while(FB_CS0==0 && FB_CS1==1);
			DelayUs(100);
			while(FB_CS0==0 && FB_CS1==1);
			BusSet(false);//bus悬空
			FB_B2=0;
			
			break;
		}
	}

}

void DataTrans(float32_t data,uint16_t scale)
{
	uint16_t out=0x00;
	out=(uint16_t)(data*scale);
	for(uint8_t i=0;i<16;i++)
	{
		PCout(i)=out&0x01;
		out>>=1;
	}
}

void SPI()
{

	
	switch(slaveAddr)
	{
		case 0:{
				FB_B0=1;
	DelayUs(500);
	while(SPI_CS0);
	uint16_t data=0;
	data=(uint16_t)(v_lv*100);
//data=0x55;
	for(uint8_t i=0;i<16;i++)
	{
		while(SPI_CLK0);
		SPI_DO0=(data>>(15-i))&0x01;
//		GPIO_SetBit(HW_GPIOC, 2);
//		GPIO_ResetBit(HW_GPIOC, 2);
		while(!SPI_CLK0);
	}
	data=(uint16_t)(conZen*1);
		for(uint8_t i=0;i<16;i++)
	{
		while(SPI_CLK0);
		SPI_DO0=(data>>(15-i))&0x01;
		while(!SPI_CLK0);
	}
	data=(uint16_t)(Q*10000);
		for(uint8_t i=0;i<16;i++)
	{
		while(SPI_CLK0);
		SPI_DO0=(data>>(15-i))&0x01;
		while(!SPI_CLK0);
	}
	SPI_DO0=0;
	FB_B0=0;
			break;
		}
		
		
		case 1:{
				FB_B1=1;
	DelayUs(500);
	while(SPI_CS1);
	uint16_t data=0;
	data=(uint16_t)(v_lv*100);
	for(uint8_t i=0;i<16;i++)
	{
		while(SPI_CLK1);
		SPI_DO1=(data>>(15-i))&0x01;

		while(!SPI_CLK1);
	}
	data=(uint16_t)(conZen*1);
		for(uint8_t i=0;i<16;i++)
	{
		while(SPI_CLK1);
		SPI_DO1=(data>>(15-i))&0x01;

		while(!SPI_CLK1);
	}
	data=(uint16_t)(Q*10000);
		for(uint8_t i=0;i<16;i++)
	{
		while(SPI_CLK1);
		SPI_DO1=(data>>(15-i))&0x01;

		while(!SPI_CLK1);
	}
	SPI_DO1=0;
	FB_B1=0;
			break;
		}
		
		
		case 2:{
				FB_B2=1;
	DelayUs(500);
	while(SPI_CS2);
	uint16_t data=0;
	data=(uint16_t)(v_lv*100);
	for(uint8_t i=0;i<16;i++)
	{
		while(SPI_CLK2);
		SPI_DO2=(data>>(15-i))&0x01;

		while(!SPI_CLK2);
	}
	data=(uint16_t)(conZen*1);
		for(uint8_t i=0;i<16;i++)
	{
		while(SPI_CLK2);
		SPI_DO2=(data>>(15-i))&0x01;

		while(!SPI_CLK2);
	}
	data=(uint16_t)(Q*10000);
		for(uint8_t i=0;i<16;i++)
	{
		while(SPI_CLK2);
		SPI_DO2=(data>>(15-i))&0x01;

		while(!SPI_CLK2);
	}
	SPI_DO2=0;
	FB_B2=0;
			break;
		}
	}
}

