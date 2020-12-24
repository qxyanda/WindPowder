#include "ff.h"

float32_t pResult=0,
					mean=0,
					Qmax=0,
					Q=0,
					pMax=0,
					pMin=0,
					v_pre=0,
					d_zeit_pre=0,
					trash=0,
					velocity=0,
					d_zeit=0,
					diff=0,
					
					conZen=0,
					v_lv=0,
					std=0,
					std_v=0,
					
					fmap_max=0;
					
					
float32_t p[dotnum],
					q[dotnum],
					
					pOut0[dotnum],//FFT1
					pOut1[dotnum];//FFT2


extern float32_t data0[dotnum],
								 data1[dotnum];
								 
uint32_t 	pIndex=0,
					count=0,
					time=0,
					N_start=0,
					mit=0,
					t_max=30,

					fmap_index=0;	

extern uint8_t dcp_ptr;
extern uint8_t dcp_ptr_pre;

extern uint32_t time,t_max;

float32_t fft(void);
void c_calc(void);
extern uint8_t full;

void AD7366(uint32_t num,uint32_t i)
{
	
//	uint32_t address1,address2;
//	address1 = SRAM_ADDRESS_BASE+4*i;
//	address2 = address1+2;

	int16_t VA1=0,VB1=0;
//	uint8_t minus_a=0,minus_b=0;
//	CS=0;//BUG
	
	
	CS=1;
	ADDR=0;
	


	CNVST=1;  
	CNVST=0;
	DelayUs(t_max);//5 
	CNVST=1;  	
//	DelayUs(1);//5
                        
	

//	while(BUSY||time>10000000);//100000

	
	while(BUSY);
	CS=0;
	

		if(Dout_A == 1)  
       VA1 |= 0x0001;  


		if(Dout_B == 1)  
       VB1 |= 0x0001;  

	
	for(uint8_t i =0;i<12;i++)	
	{
		SCLK=1;	
		VA1<<=1;
		VB1<<=1;
		

    VA1 |= Dout_A;   
    VB1 |= Dout_B;  

		DelayUs(2);//5
		SCLK=0;
	}
	
	SCLK=1;	
	CS=1;

	
	if(VA1>0x800)VA1&=0x7FF;
	else VA1|=0x800;

	if(VB1>0x800)VB1&=0x7FF;
	else VB1|=0x800;
	
	data0[i]=(float32_t)(VA1&0xFFF);//ch1 up
	data1[i]=(float32_t)(VB1&0xFFF);//ch2 down
	

//	printf("%4d,%4d\r\n",VA1,VB1);

}


float32_t fft()
{
			arm_rfft_fast_instance_f32 S;
			arm_rfft_fast_init_f32(&S, dotnum);
	
			float32_t Conzen=0;
			arm_std_f32(data0,dotnum,&std);
	
			
	
			//max
			arm_max_f32(data0,dotnum,&Conzen,&pIndex);
			Conzen-=2048;
			if(Conzen<0)Conzen=-Conzen;
			
			arm_rfft_fast_f32(&S,data0,pOut0,0);//FFT1
			arm_rfft_fast_f32(&S,data1,pOut1,0);//FFT2	
	
			//c_calc
//			fmap_index=0;
//			for(uint16_t i=2;i<4096;i++)
//			fmap_index+=data0[i]>0?data0[i]:0;
//			fmap_index/=4096;


			arm_cmplx_conj_f32(pOut1,data0,dotnum>>1);//CONJ  arm_cmplx_conj_f32(pOut1,pDst1,dotnum>>1);//CONJ

			arm_cmplx_mult_cmplx_f32(pOut0,data0,data1,dotnum>>1);//MULT_CONJ  arm_cmplx_mult_cmplx_f32(pOut0,pDst1,pDst0,dotnum>>1);//MULT_CONJ
	
			arm_rfft_fast_f32(&S,data1,data0,1);//RFFT  arm_rfft_fast_f32(&S,pDst0,Res,1);//RFFT

			arm_max_f32(data0,dotnum/2,&pResult,&pIndex);//MAX  arm_max_f32(Res,dotnum,&pResult,&pIndex);//MAX
				
			arm_mean_f32(data0,dotnum,&mean);
	
			if(mean<0)mean=-mean;
			Q=pResult/(mean+1);//

			d_zeit = pIndex;//dotnum + N_start - 

			d_zeit *= time;
			d_zeit /=	100;
			d_zeit /= (dotnum-1) ;// + N_start
			
//	SC_0_R_W=1;

//	for(uint16_t i=0;i<2048;i++)
//		printf("%f\r\n",data0[i]);	
////	DelayMs(2);
////	SC_0_R_W=0;
			
			
			return Conzen;
}


