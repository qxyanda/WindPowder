#include "ff.h"

void inc(uint8_t range,uint8_t channel)
{
	INC=1;
	RES1=1;
	RES2=1;
	UD=1;
	
	DelayUs(5);
	switch(channel)
	{
		case 0:
		{
			RES1=0;
			DelayUs(50);
		for(uint8_t i=0;i<range;i++)
		{
			INC=0;
			DelayUs(600);
			INC=1;
			DelayUs(2);
		}
	
		RES1=1;
			break;
		}
		
		
		case 1:
		{
			RES2=0;
			DelayUs(50);
			for(uint8_t i=0;i<range;i++)
			{
				INC=0;
				DelayUs(600);
				INC=1;
				DelayUs(2);
			}
			RES2=1;
			break;
		}
	}
	
	DelayMs(50);
}



void dec(uint8_t range,uint8_t channel)
{
	INC=1;
	RES1=1;
	RES2=1;
	UD=0;
	
	DelayUs(5);
	switch(channel)
	{
		case 0:
		{
			RES1=0;
			DelayUs(50);
		for(uint8_t i=0;i<range;i++)
		{
			INC=0;
			DelayUs(600);
			INC=1;
			DelayUs(2);
		}
	
		RES1=1;
			break;
		}
		
		case 1:
		{
			RES2=0;
			DelayUs(50);
			for(uint8_t i=0;i<range;i++)
			{
				INC=0;
				DelayUs(600);
				INC=1;
				DelayUs(2);
			}
			RES2=1;
			break;
		}
	}
	
	DelayMs(50);

}


void Gain(uint16_t data[])
{

	for(uint8_t i=0;i<2;i++)
	{
		if(data[i]==0x0FFF)dec(1,i);
		if(data[i]<=0x0800)inc(1,i);
	}
	
}















//有关电位器的宏定义 

#define   SETB_X9C103_CS1  PAout(10)=1
#define   CLRB_X9C103_CS1  PAout(10)=0

#define   SETB_X9C103_INC  PAout(8)=1 
#define   CLRB_X9C103_INC  PAout(8)=0 
#define   SETB_X9C103_UD   PAout(9)=1   
#define   CLRB_X9C103_UD   PAout(9)=0

#define   SETB_X9C103_CS2  PAout(11)=1
#define   CLRB_X9C103_CS2  PAout(11)=0


void X9C103_Inc_N_Step(unsigned char Sel,unsigned char N);
void X9C103_Dec_N_Step(unsigned char Sel,unsigned char N);
//void Delay(unsigned int t) ;
void X9C103_Init(unsigned char Sel);//初始化至中间位置





//************************************************************************
// 数字电位器向上调一步
// 数字电位器100个抽头，相当于99步
//************************************************************************
void X9C103_Inc_N_Step(unsigned char Sel,unsigned char N)
{
   unsigned char i=0; 
   SETB_X9C103_UD;   // U/D 拉高   则下面的INC下沿，执行UP操作
   DelayUs(3);         // 延时大约2us
   
  switch(Sel)
  {
    case 1:
 CLRB_X9C103_CS1;  
    break;
    case 2:
 CLRB_X9C103_CS2; 
    break;
    default:break;    
   } 
//   CLRB_X9C103_CS;   // CS  拉低

   for(i=N;i>0;i--) 
  { 
     SETB_X9C103_INC;  // 拉高INC    因为INC的下沿有效
     DelayUs(2);         // 延时2us左右
     CLRB_X9C103_INC;   // INC 拉低;  // 产生一个下沿
     DelayUs(600);       // 延时大约500us, Rw等待输出稳定
   }
  SETB_X9C103_INC;//保存模式
  switch(Sel)
  {
    case 1:
 SETB_X9C103_CS1;  
    break;
    case 2:
 SETB_X9C103_CS2; 
    break;
    default:break;    
   } // 完毕CS拉高 //store
}
//************************************************************************
// 数字电位器向下调一步
// 数字电位器100个抽头，相当于99步
//************************************************************************
void X9C103_Dec_N_Step(unsigned char Sel,unsigned char N)
{
   unsigned char i=0; 
   CLRB_X9C103_UD;   // U/D 清0，  则下面的INC下沿，执行Down操作
   
   DelayUs(3);         // 延时大约2us
   
   switch(Sel)
  {
    case 1:
 CLRB_X9C103_CS1;  
    break;
    case 2:
 CLRB_X9C103_CS2; 
    break;
    default:break;    
   } 
   for(i=N;i>0;i--) 
  { 
     SETB_X9C103_INC;  // 拉高INC    因为INC的下沿有效
     DelayUs(2);         // 延时2us左右
     CLRB_X9C103_INC;   // INC 拉低;  // 产生一个下沿
     DelayUs(600);       // 延时大约500us, Rw等待输出稳定
  }
  SETB_X9C103_INC;//保存模式
   switch(Sel)
  {
    case 1:
 SETB_X9C103_CS1;  
    break;
    case 2:
 SETB_X9C103_CS2; 
    break;
    default:break;    
   } // 完毕CS拉高 // store 
}

void X9C103_Init(unsigned char Sel)
{
  X9C103_Dec_N_Step(Sel,99); 
  X9C103_Inc_N_Step(Sel,49); 
} 







