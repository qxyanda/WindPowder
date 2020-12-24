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















//�йص�λ���ĺ궨�� 

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
void X9C103_Init(unsigned char Sel);//��ʼ�����м�λ��





//************************************************************************
// ���ֵ�λ�����ϵ�һ��
// ���ֵ�λ��100����ͷ���൱��99��
//************************************************************************
void X9C103_Inc_N_Step(unsigned char Sel,unsigned char N)
{
   unsigned char i=0; 
   SETB_X9C103_UD;   // U/D ����   �������INC���أ�ִ��UP����
   DelayUs(3);         // ��ʱ��Լ2us
   
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
//   CLRB_X9C103_CS;   // CS  ����

   for(i=N;i>0;i--) 
  { 
     SETB_X9C103_INC;  // ����INC    ��ΪINC��������Ч
     DelayUs(2);         // ��ʱ2us����
     CLRB_X9C103_INC;   // INC ����;  // ����һ������
     DelayUs(600);       // ��ʱ��Լ500us, Rw�ȴ�����ȶ�
   }
  SETB_X9C103_INC;//����ģʽ
  switch(Sel)
  {
    case 1:
 SETB_X9C103_CS1;  
    break;
    case 2:
 SETB_X9C103_CS2; 
    break;
    default:break;    
   } // ���CS���� //store
}
//************************************************************************
// ���ֵ�λ�����µ�һ��
// ���ֵ�λ��100����ͷ���൱��99��
//************************************************************************
void X9C103_Dec_N_Step(unsigned char Sel,unsigned char N)
{
   unsigned char i=0; 
   CLRB_X9C103_UD;   // U/D ��0��  �������INC���أ�ִ��Down����
   
   DelayUs(3);         // ��ʱ��Լ2us
   
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
     SETB_X9C103_INC;  // ����INC    ��ΪINC��������Ч
     DelayUs(2);         // ��ʱ2us����
     CLRB_X9C103_INC;   // INC ����;  // ����һ������
     DelayUs(600);       // ��ʱ��Լ500us, Rw�ȴ�����ȶ�
  }
  SETB_X9C103_INC;//����ģʽ
   switch(Sel)
  {
    case 1:
 SETB_X9C103_CS1;  
    break;
    case 2:
 SETB_X9C103_CS2; 
    break;
    default:break;    
   } // ���CS���� // store 
}

void X9C103_Init(unsigned char Sel)
{
  X9C103_Dec_N_Step(Sel,99); 
  X9C103_Inc_N_Step(Sel,49); 
} 







