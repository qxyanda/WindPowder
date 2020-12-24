/*-----------------------------------------------------------------------------------------------------*/
/*-----------------------------------------静电----------------------------------------------------*/
/*-------------------------------------2017.02.15.阎达-------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------*/

//#include "chlib_k.h"
#include "ff.h"

float32_t G=1;
//浓度标定 y=a(x-b)
float32_t a=10;
float32_t b=0;


uint8_t N =1;
//AIR
float32_t Rg=287;
float32_t P=100900;
float32_t Th=296.15;




float32_t k1=0.18;
float32_t k2=0.08956;
float32_t v_0=0;
float32_t delta=0;

uint8_t vorc =0;

//锁相环频率为50/15*54=180M测试函数
//extern void pllinit100M(void);
//extern void pllinit180M(void);
extern uint8_t ini(void);
extern void c_calc(void);

extern float32_t fft(void);

void Disp(void);
void dcp(void);
void lv_bo(void);

//void Bublesort(float32_t a[],uint8_t n);

extern void Gain(uint16_t data[]);
extern double TC_Read(void);
void c_read(void);
void cali_alpha(void);

extern uint8_t full;
extern uint8_t lock;
extern uint8_t ch[17];
extern uint8_t i;


extern float32_t pResult,
					mean,
					Qmax,
					Q,
					pMax,
					pMin,
					v_pre,
					d_zeit_pre,
					trash,
					velocity,
					d_zeit,
					diff,
					
					conZen,
					v_lv,
					std,
					std_v;
float32_t std_lv=0;
float32_t std_v_pre=0;
float32_t fmap_index_pre=0;
					
extern float32_t p[dotnum],
					q[dotnum],
					
					pOut0[dotnum],//FFT1
					pOut1[dotnum];//FFT2


extern float32_t data0[dotnum],
								 data1[dotnum];


//uint8_t cycle=1;

					
extern uint32_t 	pIndex,
					count,
					time,
					N_start,
					mit,
					t_max,

					fmap_index;	

uint32_t zeit_c=0;
uint32_t zeit_pre=0;
float32_t c_pre=0;
float32_t c=0;

extern void AD7366(uint32_t num,uint32_t i);
extern void dec(uint8_t range,uint8_t channel);
extern void inc(uint8_t range,uint8_t channel);



extern void X9C103_Inc_N_Step(unsigned char Sel,unsigned char N);
extern void X9C103_Dec_N_Step(unsigned char Sel,unsigned char N);
//void Delay(unsigned int t) ;
extern void X9C103_Init(unsigned char Sel);//初始化至中间位置


uint16_t a0=0,a1=0,NU=0;	

uint8_t dcp_ptr=50;
uint8_t dcp_ptr_pre=0;
uint8_t call=0;





//uint16_t force_1=0;
//uint16_t force_2=0;
uint16_t force=0;
uint16_t force_pre=0;





float32_t alpha_vector[8]={0.204,0.204,0.204,0.204,0.204,0.204,0.204,0.204};


uint8_t NU_vector[10];
//AIR




float32_t dm_coal=0;

float32_t force_zero=0;

float32_t v_vector[20];
float32_t vf_vector[5]={0,0,0,0,0};
float32_t ve_vector[5]={0,0,0,0,0};

	float32_t v_now=0;
	uint8_t lv_num=20;	

	uint8_t SW=0;
  uint16_t value;
	double tc;
	
	extern uint8_t buf[PAGE_SIZE];
	uint8_t cali_num=0;

	int32_t NU_sum;
	float32_t v_f;

extern uint8_t addr_slave;
extern uint8_t addr_host;
extern uint8_t isDebugMode;

extern uint8_t ch_HostRe[8];//single
extern uint8_t ch_HostOutput[9];//single




uint8_t echo_data[4];

//host

extern void data_store(void);
extern void host_output(void);
void comdata_handle(void);

void echo(void);

extern uint8_t Re;
//float32_t P_lv=0;

#define vecLength 10
float32_t vVec[vecLength];
float32_t cVec[vecLength];

void VecFil(void);

uint8_t iniCount=0;
extern uint8_t outputEn;
extern void Bus(void);
extern void SPI(void);
void PIT_ISR(void)
{
	time++;
}

void PIT_ISR1(void)
{
	zeit_c++;
}

int main(void)//<main>
{
	//初始化
  LED_INI = ini();
	RANGE0=0;
	RANGE1=0;
	
	PIT_InitTypeDef PIT_InitStruct1;  //申请结构体变量
  PIT_InitStruct1.chl = HW_PIT_CH0; /* 使用0号定时器 */
  PIT_InitStruct1.timeInUs = 10; /* 定时周期1S *///us
  PIT_Init(&PIT_InitStruct1); //pit模块初始化
  /* 注册PIT 中断回调函数 */
  PIT_CallbackInstall(HW_PIT_CH0, PIT_ISR); //0号定时器的中断处理
  /* 开启PIT0定时器中断 */
//  PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, true);

	v_lv=0;

	
	//主循环
	for(;;)//<main_loop>
	{
		t_max=5;
		
		//循环采集数据N次
		for(uint8_t i=0;i<N;i++)
		{		
			time=0;	
			PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, true);	
			for(uint32_t i=0;i<dotnum;i++)AD7366(dotnum,i);
			PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF, false);
			
			//计算延时
			fft();//
			conZen=(a*(std-b)/G)*4/3.666;//浓度系数
			if(slaveAddr==1)conZen/=bScale;
			if(slaveAddr==2)conZen/=cScale;
			
			
			
			DelayMs(10);

			float32_t  v=0;
			v=L/d_zeit;
			
			v_lv=v;
			
//			conZen=std;

			//改变采样总时长
			t_max+=25;
		}
		
	
//		v_lv=slaveAddr+1;
//		conZen=666*(slaveAddr+1);
//		Q=1.01*(slaveAddr+1);
//		v_lv+=0.01;
//		Bus();

		SPI();
		WDOG_Refresh();	

	}	//</main_loop>		

}//</main>







