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


// void pllinit60M(void);
// void pllinit80M(void);
// void pllinit100M(void);
// void pllinit125M(void);
// void pllinit1375M(void);
// //锁相环频率为50/19*55=144.7386421M测试函数
// void pllinit144M(void);
// //锁相环频率为50/18*54=150M测试函数
//void pllinit150M(void);
////锁相环频率为50/16*54=168.75M测试函数
//void pllinit168d75M(void);
////锁相环频率为50/13*54=207.7M测试函数
//void pllinit207M(void);
////锁相环频率为50/13*55=211M测试函数
//void pllinit211M(void);
////锁相环频率为50/12*54=225M测试函数
//void pllinit225M(void);
////锁相环频率为50/11*54=245.45M测试函数
//void pllinit245M(void);
////锁相环频率为50/10*54=270M测试函数
//void pllinit270M(void);

//////////////////////////////////////////////////
//超频//超频//超频//超频//超频//超频//超频//超频//
//////////////////////////////////////////////////


//锁相环频率为50/15*54=180M测试函数
void pllinit180M(void)
{
	uint32_t temp_reg;
        //使能IO端口时钟    
    SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
                              | SIM_SCGC5_PORTB_MASK
                              | SIM_SCGC5_PORTC_MASK
                              | SIM_SCGC5_PORTD_MASK
                              | SIM_SCGC5_PORTE_MASK );
    //这里处在默认的FEI模式
    //首先移动到FBE模式
    MCG_C2 = 0;  
    //MCG_C2 = MCG_C2_RANGE(2) | MCG_C2_HGO_MASK | MCG_C2_EREFS_MASK;
    //初始化晶振后释放锁定状态的振荡器和GPIO
    SIM_SCGC4 |= SIM_SCGC4_LLWU_MASK;
//    LLWU_CS |= LLWU_CS_ACKISO_MASK;
     LLWU_FILT1 |= LLWU_FILT1_FILTF_MASK;   
    
    //选择外部晶振，参考分频器，清IREFS来启动外部晶振
    //011 If RANGE = 0, Divide Factor is 8; for all other RANGE values, Divide Factor is 256.
    MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(3);
    
    //等待晶振稳定	    
    //while (!(MCG_S & MCG_S_OSCINIT_MASK)){}              //等待锁相环初始化结束
    while (MCG_S & MCG_S_IREFST_MASK){}                  //等待时钟切换到外部参考时钟
    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2){}
    
    //进入FBE模式,
    //0x18==25分频=2M,
    //0x08==15分频=3.333M 
    //0x09==16分频=3.125M,
    //0x10==17分频=2.94M 
    //0x11==18分频=2.7778M 
    //0x12==19分频=2.63M,
    //0x13==20分频=2.5M    
    MCG_C5 = MCG_C5_PRDIV0(0x0e);                
    
    //确保MCG_C6处于复位状态，禁止LOLIE、PLL、和时钟控制器，清PLL VCO分频器
    MCG_C6 = 0x0;
    
    //保存FMC_PFAPR当前的值
    temp_reg = FMC_PFAPR;
    
    //通过M&PFD置位M0PFD来禁止预取功能
    FMC_PFAPR |= FMC_PFAPR_M7PFD_MASK | FMC_PFAPR_M6PFD_MASK | FMC_PFAPR_M5PFD_MASK
                     | FMC_PFAPR_M4PFD_MASK | FMC_PFAPR_M3PFD_MASK | FMC_PFAPR_M2PFD_MASK
                     | FMC_PFAPR_M1PFD_MASK | FMC_PFAPR_M0PFD_MASK;    
    ///设置系统分频器
    //MCG=PLL, core = MCG, bus = MCG/3, FlexBus = MCG/3, Flash clock= MCG/8
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV2(2) 
                 | SIM_CLKDIV1_OUTDIV3(2) | SIM_CLKDIV1_OUTDIV4(7);       
    
    //从新存FMC_PFAPR的原始值
    FMC_PFAPR = temp_reg; 
    
    //设置VCO分频器，使能PLL为100MHz, LOLIE=0, PLLS=1, CME=0, VDIV=26
    MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV0(30);  //VDIV = 31 (x54)
                                                  //VDIV = 26 (x50)
    while (!(MCG_S & MCG_S_PLLST_MASK)){}; // wait for PLL status bit to set    
    while (!(MCG_S & MCG_S_LOCK0_MASK)){}; // Wait for LOCK bit to set    
    
    //进入PBE模式    
    //通过清零CLKS位来进入PEE模式
    // CLKS=0, FRDIV=3, IREFS=0, IRCLKEN=0, IREFSTEN=0
    MCG_C1 &= ~MCG_C1_CLKS_MASK;
    
    //等待时钟状态位更新
    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3){};
    //SIM_CLKDIV2 |= SIM_CLKDIV2_USBDIV(1);  
    
    //设置跟踪时钟为内核时钟
    SIM_SOPT2 |= SIM_SOPT2_TRACECLKSEL_MASK;	
    //在PTA6引脚上使能TRACE_CLKOU功能
    PORTA_PCR6 = ( PORT_PCR_MUX(0x7));  
    //使能FlexBus模块时钟
    SIM_SCGC7 |= SIM_SCGC7_FLEXBUS_MASK;
    //在PTA6引脚上使能FB_CLKOUT功能
    PORTC_PCR3 = ( PORT_PCR_MUX(0x5));
}





















////锁相环频率为137.5M测试函数
//void pllinit1375M(void)
//{
//	uint32_t temp_reg;
//        //使能IO端口时钟    
//    SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
//                              | SIM_SCGC5_PORTB_MASK
//                              | SIM_SCGC5_PORTC_MASK
//                              | SIM_SCGC5_PORTD_MASK
//                              | SIM_SCGC5_PORTE_MASK );
//    //这里处在默认的FEI模式
//    //首先移动到FBE模式
//    MCG_C2 = 0;  
//    //MCG_C2 = MCG_C2_RANGE(2) | MCG_C2_HGO_MASK | MCG_C2_EREFS_MASK;
//    //初始化晶振后释放锁定状态的振荡器和GPIO
//    SIM_SCGC4 |= SIM_SCGC4_LLWU_MASK;
////    LLWU_CS |= LLWU_CS_ACKISO_MASK;
//    LLWU_FILT1 |= LLWU_FILT1_FILTF_MASK;
//    //选择外部晶振，参考分频器，清IREFS来启动外部晶振
//    //011 If RANGE = 0, Divide Factor is 8; for all other RANGE values, Divide Factor is 256.
//    MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(3);
//    
//    //等待晶振稳定	    
//    //while (!(MCG_S & MCG_S_OSCINIT_MASK)){}              //等待锁相环初始化结束
//    while (MCG_S & MCG_S_IREFST_MASK){}                  //等待时钟切换到外部参考时钟
//    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2){}
//    
//    //进入FBE模式,
//    //0x18==25分频=2M,
//    //0x11==18分频=2.7778M 
//    //0x12==19分频=2.63M,
//    //0x13==20分频=2.5M    
//    MCG_C5 = MCG_C5_PRDIV0(0x13);                
//    
//    //确保MCG_C6处于复位状态，禁止LOLIE、PLL、和时钟控制器，清PLL VCO分频器
//    MCG_C6 = 0x0;
//    
//    //保存FMC_PFAPR当前的值
//    temp_reg = FMC_PFAPR;
//    
//    //通过M&PFD置位M0PFD来禁止预取功能
//    FMC_PFAPR |= FMC_PFAPR_M7PFD_MASK | FMC_PFAPR_M6PFD_MASK | FMC_PFAPR_M5PFD_MASK
//                     | FMC_PFAPR_M4PFD_MASK | FMC_PFAPR_M3PFD_MASK | FMC_PFAPR_M2PFD_MASK
//                     | FMC_PFAPR_M1PFD_MASK | FMC_PFAPR_M0PFD_MASK;    
//    ///设置系统分频器
//    //MCG=PLL, core = MCG, bus = MCG/2, FlexBus = MCG/2, Flash clock= MCG/4
//    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV2(1) 
//                 | SIM_CLKDIV1_OUTDIV3(1) | SIM_CLKDIV1_OUTDIV4(3);       
//    
//    //从新存FMC_PFAPR的原始值
//    FMC_PFAPR = temp_reg; 
//    
//    //设置VCO分频器，使能PLL为100MHz, LOLIE=0, PLLS=1, CME=0, VDIV=26
//    MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV0(31);  //VDIV = 31 (x55)
//                                                  //VDIV = 26 (x50)
//    while (!(MCG_S & MCG_S_PLLST_MASK)){}; // wait for PLL status bit to set    
//    while (!(MCG_S & MCG_S_LOCK0_MASK)){}; // Wait for LOCK bit to set    
//    
//    //进入PBE模式    
//    //通过清零CLKS位来进入PEE模式
//    // CLKS=0, FRDIV=3, IREFS=0, IRCLKEN=0, IREFSTEN=0
//    MCG_C1 &= ~MCG_C1_CLKS_MASK;
//    
//    //等待时钟状态位更新
//    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3){};
//    //SIM_CLKDIV2 |= SIM_CLKDIV2_USBDIV(1);  
//    
//    //设置跟踪时钟为内核时钟
//    SIM_SOPT2 |= SIM_SOPT2_TRACECLKSEL_MASK;	
//    //在PTA6引脚上使能TRACE_CLKOU功能
//    PORTA_PCR6 = ( PORT_PCR_MUX(0x7));  
//    //使能FlexBus模块时钟
//    SIM_SCGC7 |= SIM_SCGC7_FLEXBUS_MASK;
//    //在PTA6引脚上使能FB_CLKOUT功能
//    PORTC_PCR3 = ( PORT_PCR_MUX(0x5));
//}
////锁相环频率为125M测试函数
//void pllinit125M(void)
//{
//	uint32_t temp_reg;
//        //使能IO端口时钟    
//    SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
//                              | SIM_SCGC5_PORTB_MASK
//                              | SIM_SCGC5_PORTC_MASK
//                              | SIM_SCGC5_PORTD_MASK
//                              | SIM_SCGC5_PORTE_MASK );
//    //这里处在默认的FEI模式
//    //首先移动到FBE模式
//    MCG_C2 = 0;  
//    //MCG_C2 = MCG_C2_RANGE(2) | MCG_C2_HGO_MASK | MCG_C2_EREFS_MASK;
//    //初始化晶振后释放锁定状态的振荡器和GPIO
//    SIM_SCGC4 |= SIM_SCGC4_LLWU_MASK;
////    LLWU_CS |= LLWU_CS_ACKISO_MASK;
//	  LLWU_FILT1 |= LLWU_FILT1_FILTF_MASK;
//    
//    //选择外部晶振，参考分频器，清IREFS来启动外部晶振
//    //011 If RANGE = 0, Divide Factor is 8; for all other RANGE values, Divide Factor is 256.
//    MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(3);
//    
//    //等待晶振稳定	    
//    //while (!(MCG_S & MCG_S_OSCINIT_MASK)){}              //等待锁相环初始化结束
//    while (MCG_S & MCG_S_IREFST_MASK){}                  //等待时钟切换到外部参考时钟
//    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2){}
//    
//    //进入FBE模式,
//    //0x18==25分频=2M,
//    //0x11==18分频=2.7778M 
//    //0x12==19分频=2.63M,
//    //0x13==20分频=2.5M    
//    MCG_C5 = MCG_C5_PRDIV0(0x13);                
//    
//    //确保MCG_C6处于复位状态，禁止LOLIE、PLL、和时钟控制器，清PLL VCO分频器
//    MCG_C6 = 0x0;
//    
//    //保存FMC_PFAPR当前的值
//    temp_reg = FMC_PFAPR;
//    
//    //通过M&PFD置位M0PFD来禁止预取功能
//    FMC_PFAPR |= FMC_PFAPR_M7PFD_MASK | FMC_PFAPR_M6PFD_MASK | FMC_PFAPR_M5PFD_MASK
//                     | FMC_PFAPR_M4PFD_MASK | FMC_PFAPR_M3PFD_MASK | FMC_PFAPR_M2PFD_MASK
//                     | FMC_PFAPR_M1PFD_MASK | FMC_PFAPR_M0PFD_MASK;    
//    ///设置系统分频器
//    //MCG=PLL, core = MCG, bus = MCG/2, FlexBus = MCG/2, Flash clock= MCG/4
//    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV2(1) 
//                 | SIM_CLKDIV1_OUTDIV3(1) | SIM_CLKDIV1_OUTDIV4(3);       
//    
//    //从新存FMC_PFAPR的原始值
//    FMC_PFAPR = temp_reg; 
//    
//    //设置VCO分频器，使能PLL为100MHz, LOLIE=0, PLLS=1, CME=0, VDIV=26
//    MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV0(26);  //VDIV = 31 (x55)
//                                                  //VDIV = 26 (x50)
//    while (!(MCG_S & MCG_S_PLLST_MASK)){}; // wait for PLL status bit to set    
//    while (!(MCG_S & MCG_S_LOCK0_MASK)){}; // Wait for LOCK bit to set    
//    
//    //进入PBE模式    
//    //通过清零CLKS位来进入PEE模式
//    // CLKS=0, FRDIV=3, IREFS=0, IRCLKEN=0, IREFSTEN=0
//    MCG_C1 &= ~MCG_C1_CLKS_MASK;
//    
//    //等待时钟状态位更新
//    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3){};
//    //SIM_CLKDIV2 |= SIM_CLKDIV2_USBDIV(1); 
//    
//        //设置跟踪时钟为内核时钟
//    SIM_SOPT2 |= SIM_SOPT2_TRACECLKSEL_MASK;	
//    //在PTA6引脚上使能TRACE_CLKOU功能
//    PORTA_PCR6 = ( PORT_PCR_MUX(0x7));  
//    //使能FlexBus模块时钟
//    SIM_SCGC7 |= SIM_SCGC7_FLEXBUS_MASK;
//    //在PTA6引脚上使能FB_CLKOUT功能
//    PORTC_PCR3 = ( PORT_PCR_MUX(0x5));
//}

//锁相环频率为100M测试函数
void pllinit100M(void)
{
	uint32_t temp_reg;
        //使能IO端口时钟    
    SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
                              | SIM_SCGC5_PORTB_MASK
                              | SIM_SCGC5_PORTC_MASK
                              | SIM_SCGC5_PORTD_MASK
                              | SIM_SCGC5_PORTE_MASK );
    //这里处在默认的FEI模式
    //首先移动到FBE模式
    MCG_C2 = 0;  
    //MCG_C2 = MCG_C2_RANGE(2) | MCG_C2_HGO_MASK | MCG_C2_EREFS_MASK;
    //初始化晶振后释放锁定状态的振荡器和GPIO
    SIM_SCGC4 |= SIM_SCGC4_LLWU_MASK;
//    LLWU_CS |= LLWU_CS_ACKISO_MASK;
    LLWU_FILT1 |= LLWU_FILT1_FILTF_MASK;
    //选择外部晶振，参考分频器，清IREFS来启动外部晶振
    //011 If RANGE = 0, Divide Factor is 8; for all other RANGE values, Divide Factor is 256.
    MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(3);
    
    //等待晶振稳定	    
    //while (!(MCG_S & MCG_S_OSCINIT_MASK)){}              //等待锁相环初始化结束
    while (MCG_S & MCG_S_IREFST_MASK){}                  //等待时钟切换到外部参考时钟
    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2){}
    
    //进入FBE模式,
    //0x18==25分频=2M,
    //0x11==18分频=2.7778M 
    //0x12==19分频=2.63M,
    //0x13==20分频=2.5M    
    MCG_C5 = MCG_C5_PRDIV0(0x18);                
    
    //确保MCG_C6处于复位状态，禁止LOLIE、PLL、和时钟控制器，清PLL VCO分频器
    MCG_C6 = 0x0;
    
    //保存FMC_PFAPR当前的值
    temp_reg = FMC_PFAPR;
    
    //通过M&PFD置位M0PFD来禁止预取功能
    FMC_PFAPR |= FMC_PFAPR_M7PFD_MASK | FMC_PFAPR_M6PFD_MASK | FMC_PFAPR_M5PFD_MASK
                     | FMC_PFAPR_M4PFD_MASK | FMC_PFAPR_M3PFD_MASK | FMC_PFAPR_M2PFD_MASK
                     | FMC_PFAPR_M1PFD_MASK | FMC_PFAPR_M0PFD_MASK;    
    ///设置系统分频器
    //MCG=PLL, core = MCG, bus = MCG/2, FlexBus = MCG/2, Flash clock= MCG/4
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV2(1) 
                 | SIM_CLKDIV1_OUTDIV3(1) | SIM_CLKDIV1_OUTDIV4(3);       
    
    //从新存FMC_PFAPR的原始值
    FMC_PFAPR = temp_reg; 
    
    //设置VCO分频器，使能PLL为100MHz, LOLIE=0, PLLS=1, CME=0, VDIV=26
    MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV0(26);  //VDIV = 31 (x55)
                                                  //VDIV = 26 (x50)
    while (!(MCG_S & MCG_S_PLLST_MASK)){}; // wait for PLL status bit to set    
    while (!(MCG_S & MCG_S_LOCK0_MASK)){}; // Wait for LOCK bit to set    
    
    //进入PBE模式    
    //通过清零CLKS位来进入PEE模式
    // CLKS=0, FRDIV=3, IREFS=0, IRCLKEN=0, IREFSTEN=0
    MCG_C1 &= ~MCG_C1_CLKS_MASK;
    
    //等待时钟状态位更新
    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3){};
    //SIM_CLKDIV2 |= SIM_CLKDIV2_USBDIV(1);  
    
        //设置跟踪时钟为内核时钟
    SIM_SOPT2 |= SIM_SOPT2_TRACECLKSEL_MASK;	
    //在PTA6引脚上使能TRACE_CLKOU功能
    PORTA_PCR6 = ( PORT_PCR_MUX(0x7));  
    //使能FlexBus模块时钟
    SIM_SCGC7 |= SIM_SCGC7_FLEXBUS_MASK;
    //在PTA6引脚上使能FB_CLKOUT功能
    PORTC_PCR3 = ( PORT_PCR_MUX(0x5));
}

////锁相环频率为80M测试函数
//void pllinit80M(void)
//{
//	uint32_t temp_reg;
//        //使能IO端口时钟    
//    SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
//                              | SIM_SCGC5_PORTB_MASK
//                              | SIM_SCGC5_PORTC_MASK
//                              | SIM_SCGC5_PORTD_MASK
//                              | SIM_SCGC5_PORTE_MASK );
//    //这里处在默认的FEI模式
//    //首先移动到FBE模式
//    MCG_C2 = 0;  
//    //MCG_C2 = MCG_C2_RANGE(2) | MCG_C2_HGO_MASK | MCG_C2_EREFS_MASK;
//    //初始化晶振后释放锁定状态的振荡器和GPIO
//    SIM_SCGC4 |= SIM_SCGC4_LLWU_MASK;
////    LLWU_CS |= LLWU_CS_ACKISO_MASK;
//    LLWU_FILT1 |= LLWU_FILT1_FILTF_MASK;
//    //选择外部晶振，参考分频器，清IREFS来启动外部晶振
//    //011 If RANGE = 0, Divide Factor is 8; for all other RANGE values, Divide Factor is 256.
//    MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(3);
//    
//    //等待晶振稳定	    
//    //while (!(MCG_S & MCG_S_OSCINIT_MASK)){}              //等待锁相环初始化结束
//    while (MCG_S & MCG_S_IREFST_MASK){}                  //等待时钟切换到外部参考时钟
//    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2){}
//    
//    //进入FBE模式,
//    //0x18==25分频=2M,
//    //0x11==18分频=2.7778M 
//    //0x12==19分频=2.63M,
//    //0x13==20分频=2.5M    
//    MCG_C5 = MCG_C5_PRDIV0(0x18);                
//    
//    //确保MCG_C6处于复位状态，禁止LOLIE、PLL、和时钟控制器，清PLL VCO分频器
//    MCG_C6 = 0x0;
//    
//    //保存FMC_PFAPR当前的值
//    temp_reg = FMC_PFAPR;
//    
//    //通过M&PFD置位M0PFD来禁止预取功能
//    FMC_PFAPR |= FMC_PFAPR_M7PFD_MASK | FMC_PFAPR_M6PFD_MASK | FMC_PFAPR_M5PFD_MASK
//                     | FMC_PFAPR_M4PFD_MASK | FMC_PFAPR_M3PFD_MASK | FMC_PFAPR_M2PFD_MASK
//                     | FMC_PFAPR_M1PFD_MASK | FMC_PFAPR_M0PFD_MASK;    
//    ///设置系统分频器
//    //MCG=PLL, core = MCG, bus = MCG/2, FlexBus = MCG/2, Flash clock= MCG/4
//    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV2(1) 
//                 | SIM_CLKDIV1_OUTDIV3(1) | SIM_CLKDIV1_OUTDIV4(3);       
//    
//    //从新存FMC_PFAPR的原始值
//    FMC_PFAPR = temp_reg; 
//    
//    //设置VCO分频器，使能PLL为100MHz, LOLIE=0, PLLS=1, CME=0, VDIV=26
//    MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV0(16);  //VDIV = 31 (x55)
//                                                  //VDIV = 26 (x50)
//												  //VDIV = 16 (x40)
//    while (!(MCG_S & MCG_S_PLLST_MASK)){}; // wait for PLL status bit to set    
//    while (!(MCG_S & MCG_S_LOCK0_MASK)){}; // Wait for LOCK bit to set    
//    
//    //进入PBE模式    
//    //通过清零CLKS位来进入PEE模式
//    // CLKS=0, FRDIV=3, IREFS=0, IRCLKEN=0, IREFSTEN=0
//    MCG_C1 &= ~MCG_C1_CLKS_MASK;
//    
//    //等待时钟状态位更新
//    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3){};
//    //SIM_CLKDIV2 |= SIM_CLKDIV2_USBDIV(1);    
//    
//        //设置跟踪时钟为内核时钟
//    SIM_SOPT2 |= SIM_SOPT2_TRACECLKSEL_MASK;	
//    //在PTA6引脚上使能TRACE_CLKOU功能
//    PORTA_PCR6 = ( PORT_PCR_MUX(0x7));  
//    //使能FlexBus模块时钟
//    SIM_SCGC7 |= SIM_SCGC7_FLEXBUS_MASK;
//    //在PTA6引脚上使能FB_CLKOUT功能
//    PORTC_PCR3 = ( PORT_PCR_MUX(0x5));
//}

////锁相环频率为60M测试函数
//void pllinit60M(void)
//{
//	uint32_t temp_reg;
//        //使能IO端口时钟    
//    SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
//                              | SIM_SCGC5_PORTB_MASK
//                              | SIM_SCGC5_PORTC_MASK
//                              | SIM_SCGC5_PORTD_MASK
//                              | SIM_SCGC5_PORTE_MASK );
//    //这里处在默认的FEI模式
//    //首先移动到FBE模式
//    MCG_C2 = 0;  
//    //MCG_C2 = MCG_C2_RANGE(2) | MCG_C2_HGO_MASK | MCG_C2_EREFS_MASK;
//    //初始化晶振后释放锁定状态的振荡器和GPIO
//    SIM_SCGC4 |= SIM_SCGC4_LLWU_MASK;
////    LLWU_CS |= LLWU_CS_ACKISO_MASK;
//    LLWU_FILT1 |= LLWU_FILT1_FILTF_MASK;
//    //选择外部晶振，参考分频器，清IREFS来启动外部晶振
//    //011 If RANGE = 0, Divide Factor is 8; for all other RANGE values, Divide Factor is 256.
//    MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(3);
//    
//    //等待晶振稳定	    
//    //while (!(MCG_S & MCG_S_OSCINIT_MASK)){}              //等待锁相环初始化结束
//    while (MCG_S & MCG_S_IREFST_MASK){}                  //等待时钟切换到外部参考时钟
//    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2){}
//    
//    //进入FBE模式,
//    //0x18==25分频=2M,
//    //0x11==18分频=2.7778M 
//    //0x12==19分频=2.63M,
//    //0x13==20分频=2.5M    
//    MCG_C5 = MCG_C5_PRDIV0(0x18);                
//    
//    //确保MCG_C6处于复位状态，禁止LOLIE、PLL、和时钟控制器，清PLL VCO分频器
//    MCG_C6 = 0x0;
//    
//    //保存FMC_PFAPR当前的值
//    temp_reg = FMC_PFAPR;
//    
//    //通过M&PFD置位M0PFD来禁止预取功能
//    FMC_PFAPR |= FMC_PFAPR_M7PFD_MASK | FMC_PFAPR_M6PFD_MASK | FMC_PFAPR_M5PFD_MASK
//                     | FMC_PFAPR_M4PFD_MASK | FMC_PFAPR_M3PFD_MASK | FMC_PFAPR_M2PFD_MASK
//                     | FMC_PFAPR_M1PFD_MASK | FMC_PFAPR_M0PFD_MASK;    
//    ///设置系统分频器
//    //MCG=PLL, core = MCG, bus = MCG/2, FlexBus = MCG/2, Flash clock= MCG/4
//    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV2(1) 
//                 | SIM_CLKDIV1_OUTDIV3(1) | SIM_CLKDIV1_OUTDIV4(3);       
//    
//    //从新存FMC_PFAPR的原始值
//    FMC_PFAPR = temp_reg; 
//    
//    //设置VCO分频器，使能PLL为100MHz, LOLIE=0, PLLS=1, CME=0, VDIV=26
//    MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV0(6);  //VDIV = 31 (x55)
//                                                  //VDIV = 26 (x50)
//												  //VDIV = 6 (x30)
//    while (!(MCG_S & MCG_S_PLLST_MASK)){}; // wait for PLL status bit to set    
//    while (!(MCG_S & MCG_S_LOCK0_MASK)){}; // Wait for LOCK bit to set    
//    
//    //进入PBE模式    
//    //通过清零CLKS位来进入PEE模式
//    // CLKS=0, FRDIV=3, IREFS=0, IRCLKEN=0, IREFSTEN=0
//    MCG_C1 &= ~MCG_C1_CLKS_MASK;
//    
//    //等待时钟状态位更新
//    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3){};
//    //SIM_CLKDIV2 |= SIM_CLKDIV2_USBDIV(1);  
//    
//        //设置跟踪时钟为内核时钟
//    SIM_SOPT2 |= SIM_SOPT2_TRACECLKSEL_MASK;	
//    //在PTA6引脚上使能TRACE_CLKOU功能
//    PORTA_PCR6 = ( PORT_PCR_MUX(0x7));  
//    //使能FlexBus模块时钟
//    SIM_SCGC7 |= SIM_SCGC7_FLEXBUS_MASK;
//    //在PTA6引脚上使能FB_CLKOUT功能
//    PORTC_PCR3 = ( PORT_PCR_MUX(0x5));
//}









////锁相环频率为50/10*54=270M测试函数
//void pllinit270M(void)
//{
//	uint32_t temp_reg;
//        //使能IO端口时钟    
//    SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
//                              | SIM_SCGC5_PORTB_MASK
//                              | SIM_SCGC5_PORTC_MASK
//                              | SIM_SCGC5_PORTD_MASK
//                              | SIM_SCGC5_PORTE_MASK );
//    //这里处在默认的FEI模式
//    //首先移动到FBE模式
//    MCG_C2 = 0;  
//    //MCG_C2 = MCG_C2_RANGE(2) | MCG_C2_HGO_MASK | MCG_C2_EREFS_MASK;
//    //初始化晶振后释放锁定状态的振荡器和GPIO
//    SIM_SCGC4 |= SIM_SCGC4_LLWU_MASK;
////    LLWU_CS |= LLWU_CS_ACKISO_MASK;
//     LLWU_FILT1 |= LLWU_FILT1_FILTF_MASK;   
//    //选择外部晶振，参考分频器，清IREFS来启动外部晶振
//    //011 If RANGE = 0, Divide Factor is 8; for all other RANGE values, Divide Factor is 256.
//    MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(3);
//    
//    //等待晶振稳定	    
//    //while (!(MCG_S & MCG_S_OSCINIT_MASK)){}              //等待锁相环初始化结束
//    while (MCG_S & MCG_S_IREFST_MASK){}                  //等待时钟切换到外部参考时钟
//    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2){}
//    
//    //进入FBE模式,
//    //0x18==25分频=2M,
//    //0x08==15分频=3.333M 
//    //0x09==16分频=3.125M,
//    //0x10==17分频=2.94M 
//    //0x11==18分频=2.7778M 
//    //0x12==19分频=2.63M,
//    //0x13==20分频=2.5M    
//    MCG_C5 = MCG_C5_PRDIV0(0x0C);                
//    
//    //确保MCG_C6处于复位状态，禁止LOLIE、PLL、和时钟控制器，清PLL VCO分频器
//    MCG_C6 = 0x0;
//    
//    //保存FMC_PFAPR当前的值
//    temp_reg = FMC_PFAPR;
//    
//    //通过M&PFD置位M0PFD来禁止预取功能
//    FMC_PFAPR |= FMC_PFAPR_M7PFD_MASK | FMC_PFAPR_M6PFD_MASK | FMC_PFAPR_M5PFD_MASK
//                     | FMC_PFAPR_M4PFD_MASK | FMC_PFAPR_M3PFD_MASK | FMC_PFAPR_M2PFD_MASK
//                     | FMC_PFAPR_M1PFD_MASK | FMC_PFAPR_M0PFD_MASK;    
//    ///设置系统分频器
//    //MCG=PLL, core = MCG, bus = MCG/3, FlexBus = MCG/3, Flash clock= MCG/10
//    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV2(2) 
//                 | SIM_CLKDIV1_OUTDIV3(2) | SIM_CLKDIV1_OUTDIV4(9);       
//    
//    //从新存FMC_PFAPR的原始值
//    FMC_PFAPR = temp_reg; 
//    
//    //设置VCO分频器，使能PLL为100MHz, LOLIE=0, PLLS=1, CME=0, VDIV=26
//    MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV0(30);  //VDIV = 31 (x54)
//                                                  //VDIV = 26 (x50)
//    while (!(MCG_S & MCG_S_PLLST_MASK)){}; // wait for PLL status bit to set    
//    while (!(MCG_S & MCG_S_LOCK0_MASK)){}; // Wait for LOCK bit to set    
//    
//    //进入PBE模式    
//    //通过清零CLKS位来进入PEE模式
//    // CLKS=0, FRDIV=3, IREFS=0, IRCLKEN=0, IREFSTEN=0
//    MCG_C1 &= ~MCG_C1_CLKS_MASK;
//    
//    //等待时钟状态位更新
//    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3){};
//    //SIM_CLKDIV2 |= SIM_CLKDIV2_USBDIV(1);  
//    
//    //设置跟踪时钟为内核时钟
//    SIM_SOPT2 |= SIM_SOPT2_TRACECLKSEL_MASK;	
//    //在PTA6引脚上使能TRACE_CLKOU功能
//    PORTA_PCR6 = ( PORT_PCR_MUX(0x7));  
//    //使能FlexBus模块时钟
//    SIM_SCGC7 |= SIM_SCGC7_FLEXBUS_MASK;
//    //在PTA6引脚上使能FB_CLKOUT功能
//    PORTC_PCR3 = ( PORT_PCR_MUX(0x5));
//}
////锁相环频率为50/11*54=245.45M测试函数
//void pllinit245M(void)
//{
//	uint32_t temp_reg;
//        //使能IO端口时钟    
//    SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
//                              | SIM_SCGC5_PORTB_MASK
//                              | SIM_SCGC5_PORTC_MASK
//                              | SIM_SCGC5_PORTD_MASK
//                              | SIM_SCGC5_PORTE_MASK );
//    //这里处在默认的FEI模式
//    //首先移动到FBE模式
//    MCG_C2 = 0;  
//    //MCG_C2 = MCG_C2_RANGE(2) | MCG_C2_HGO_MASK | MCG_C2_EREFS_MASK;
//    //初始化晶振后释放锁定状态的振荡器和GPIO
//    SIM_SCGC4 |= SIM_SCGC4_LLWU_MASK;
////    LLWU_CS |= LLWU_CS_ACKISO_MASK;
//     LLWU_FILT1 |= LLWU_FILT1_FILTF_MASK;   
//    
//    //选择外部晶振，参考分频器，清IREFS来启动外部晶振
//    //011 If RANGE = 0, Divide Factor is 8; for all other RANGE values, Divide Factor is 256.
//    MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(3);
//    
//    //等待晶振稳定	    
//    //while (!(MCG_S & MCG_S_OSCINIT_MASK)){}              //等待锁相环初始化结束
//    while (MCG_S & MCG_S_IREFST_MASK){}                  //等待时钟切换到外部参考时钟
//    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2){}
//    
//    //进入FBE模式,
//    //0x18==25分频=2M,
//    //0x08==15分频=3.333M 
//    //0x09==16分频=3.125M,
//    //0x10==17分频=2.94M 
//    //0x11==18分频=2.7778M 
//    //0x12==19分频=2.63M,
//    //0x13==20分频=2.5M    
//    MCG_C5 = MCG_C5_PRDIV0(0x0a);                
//    
//    //确保MCG_C6处于复位状态，禁止LOLIE、PLL、和时钟控制器，清PLL VCO分频器
//    MCG_C6 = 0x0;
//    
//    //保存FMC_PFAPR当前的值
//    temp_reg = FMC_PFAPR;
//    
//    //通过M&PFD置位M0PFD来禁止预取功能
//    FMC_PFAPR |= FMC_PFAPR_M7PFD_MASK | FMC_PFAPR_M6PFD_MASK | FMC_PFAPR_M5PFD_MASK
//                     | FMC_PFAPR_M4PFD_MASK | FMC_PFAPR_M3PFD_MASK | FMC_PFAPR_M2PFD_MASK
//                     | FMC_PFAPR_M1PFD_MASK | FMC_PFAPR_M0PFD_MASK;    
//    ///设置系统分频器
//    //MCG=PLL, core = MCG, bus = MCG/3, FlexBus = MCG/3, Flash clock= MCG/10
//    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV2(2) 
//                 | SIM_CLKDIV1_OUTDIV3(2) | SIM_CLKDIV1_OUTDIV4(9);       
//    
//    //从新存FMC_PFAPR的原始值
//    FMC_PFAPR = temp_reg; 
//    
//    //设置VCO分频器，使能PLL为100MHz, LOLIE=0, PLLS=1, CME=0, VDIV=26
//    MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV0(30);  //VDIV = 31 (x54)
//                                                  //VDIV = 26 (x50)
//    while (!(MCG_S & MCG_S_PLLST_MASK)){}; // wait for PLL status bit to set    
//    while (!(MCG_S & MCG_S_LOCK0_MASK)){}; // Wait for LOCK bit to set    
//    
//    //进入PBE模式    
//    //通过清零CLKS位来进入PEE模式
//    // CLKS=0, FRDIV=3, IREFS=0, IRCLKEN=0, IREFSTEN=0
//    MCG_C1 &= ~MCG_C1_CLKS_MASK;
//    
//    //等待时钟状态位更新
//    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3){};
//    //SIM_CLKDIV2 |= SIM_CLKDIV2_USBDIV(1);  
//    
//    //设置跟踪时钟为内核时钟
//    SIM_SOPT2 |= SIM_SOPT2_TRACECLKSEL_MASK;	
//    //在PTA6引脚上使能TRACE_CLKOU功能
//    PORTA_PCR6 = ( PORT_PCR_MUX(0x7));  
//    //使能FlexBus模块时钟
//    SIM_SCGC7 |= SIM_SCGC7_FLEXBUS_MASK;
//    //在PTA6引脚上使能FB_CLKOUT功能
//    PORTC_PCR3 = ( PORT_PCR_MUX(0x5));
//}
////锁相环频率为50/12*54=225M测试函数
//void pllinit225M(void)
//{
//	uint32_t temp_reg;
//        //使能IO端口时钟    
//    SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
//                              | SIM_SCGC5_PORTB_MASK
//                              | SIM_SCGC5_PORTC_MASK
//                              | SIM_SCGC5_PORTD_MASK
//                              | SIM_SCGC5_PORTE_MASK );
//    //这里处在默认的FEI模式
//    //首先移动到FBE模式
//    MCG_C2 = 0;  
//    //MCG_C2 = MCG_C2_RANGE(2) | MCG_C2_HGO_MASK | MCG_C2_EREFS_MASK;
//    //初始化晶振后释放锁定状态的振荡器和GPIO
//    SIM_SCGC4 |= SIM_SCGC4_LLWU_MASK;
////    LLWU_CS |= LLWU_CS_ACKISO_MASK;
//     LLWU_FILT1 |= LLWU_FILT1_FILTF_MASK;   
//    
//    //选择外部晶振，参考分频器，清IREFS来启动外部晶振
//    //011 If RANGE = 0, Divide Factor is 8; for all other RANGE values, Divide Factor is 256.
//    MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(3);
//    
//    //等待晶振稳定	    
//    //while (!(MCG_S & MCG_S_OSCINIT_MASK)){}              //等待锁相环初始化结束
//    while (MCG_S & MCG_S_IREFST_MASK){}                  //等待时钟切换到外部参考时钟
//    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2){}
//    
//    //进入FBE模式,
//    //0x18==25分频=2M,
//    //0x08==15分频=3.333M 
//    //0x09==16分频=3.125M,
//    //0x10==17分频=2.94M 
//    //0x11==18分频=2.7778M 
//    //0x12==19分频=2.63M,
//    //0x13==20分频=2.5M    
//    MCG_C5 = MCG_C5_PRDIV0(0x0b);                
//    
//    //确保MCG_C6处于复位状态，禁止LOLIE、PLL、和时钟控制器，清PLL VCO分频器
//    MCG_C6 = 0x0;
//    
//    //保存FMC_PFAPR当前的值
//    temp_reg = FMC_PFAPR;
//    
//    //通过M&PFD置位M0PFD来禁止预取功能
//    FMC_PFAPR |= FMC_PFAPR_M7PFD_MASK | FMC_PFAPR_M6PFD_MASK | FMC_PFAPR_M5PFD_MASK
//                     | FMC_PFAPR_M4PFD_MASK | FMC_PFAPR_M3PFD_MASK | FMC_PFAPR_M2PFD_MASK
//                     | FMC_PFAPR_M1PFD_MASK | FMC_PFAPR_M0PFD_MASK;    
//    ///设置系统分频器
//    //MCG=PLL, core = MCG, bus = MCG/3, FlexBus = MCG/3, Flash clock= MCG/9
//    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV2(2) 
//                 | SIM_CLKDIV1_OUTDIV3(2) | SIM_CLKDIV1_OUTDIV4(8);       
//    
//    //从新存FMC_PFAPR的原始值
//    FMC_PFAPR = temp_reg; 
//    
//    //设置VCO分频器，使能PLL为100MHz, LOLIE=0, PLLS=1, CME=0, VDIV=26
//    MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV0(30);  //VDIV = 31 (x54)
//                                                  //VDIV = 26 (x50)
//    while (!(MCG_S & MCG_S_PLLST_MASK)){}; // wait for PLL status bit to set    
//    while (!(MCG_S & MCG_S_LOCK0_MASK)){}; // Wait for LOCK bit to set    
//    
//    //进入PBE模式    
//    //通过清零CLKS位来进入PEE模式
//    // CLKS=0, FRDIV=3, IREFS=0, IRCLKEN=0, IREFSTEN=0
//    MCG_C1 &= ~MCG_C1_CLKS_MASK;
//    
//    //等待时钟状态位更新
//    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3){};
//    //SIM_CLKDIV2 |= SIM_CLKDIV2_USBDIV(1);  
//    
//    //设置跟踪时钟为内核时钟
//    SIM_SOPT2 |= SIM_SOPT2_TRACECLKSEL_MASK;	
//    //在PTA6引脚上使能TRACE_CLKOU功能
//    PORTA_PCR6 = ( PORT_PCR_MUX(0x7));  
//    //使能FlexBus模块时钟
//    SIM_SCGC7 |= SIM_SCGC7_FLEXBUS_MASK;
//    //在PTA6引脚上使能FB_CLKOUT功能
//    PORTC_PCR3 = ( PORT_PCR_MUX(0x5));
//}
////锁相环频率为50/13*55=211.538462M测试函数
//void pllinit211M(void)
//{
//	uint32_t temp_reg;
//        //使能IO端口时钟    
//    SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
//                              | SIM_SCGC5_PORTB_MASK
//                              | SIM_SCGC5_PORTC_MASK
//                              | SIM_SCGC5_PORTD_MASK
//                              | SIM_SCGC5_PORTE_MASK );
//    //这里处在默认的FEI模式
//    //首先移动到FBE模式
//    MCG_C2 = 0;  
//    //MCG_C2 = MCG_C2_RANGE(2) | MCG_C2_HGO_MASK | MCG_C2_EREFS_MASK;
//    //初始化晶振后释放锁定状态的振荡器和GPIO
//    SIM_SCGC4 |= SIM_SCGC4_LLWU_MASK;
////    LLWU_CS |= LLWU_CS_ACKISO_MASK;
//     LLWU_FILT1 |= LLWU_FILT1_FILTF_MASK;   
//    
//    //选择外部晶振，参考分频器，清IREFS来启动外部晶振
//    //011 If RANGE = 0, Divide Factor is 8; for all other RANGE values, Divide Factor is 256.
//    MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(3);
//    
//    //等待晶振稳定	    
//    //while (!(MCG_S & MCG_S_OSCINIT_MASK)){}              //等待锁相环初始化结束
//    while (MCG_S & MCG_S_IREFST_MASK){}                  //等待时钟切换到外部参考时钟
//    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2){}
//    
//    //进入FBE模式,
//    //0x18==25分频=2M,
//    //0x08==15分频=3.333M 
//    //0x09==16分频=3.125M,
//    //0x10==17分频=2.94M 
//    //0x11==18分频=2.7778M 
//    //0x12==19分频=2.63M,
//    //0x13==20分频=2.5M    
//    MCG_C5 = MCG_C5_PRDIV0(0x0C);                
//    
//    //确保MCG_C6处于复位状态，禁止LOLIE、PLL、和时钟控制器，清PLL VCO分频器
//    MCG_C6 = 0x0;
//    
//    //保存FMC_PFAPR当前的值
//    temp_reg = FMC_PFAPR;
//    
//    //通过M&PFD置位M0PFD来禁止预取功能
//    FMC_PFAPR |= FMC_PFAPR_M7PFD_MASK | FMC_PFAPR_M6PFD_MASK | FMC_PFAPR_M5PFD_MASK
//                     | FMC_PFAPR_M4PFD_MASK | FMC_PFAPR_M3PFD_MASK | FMC_PFAPR_M2PFD_MASK
//                     | FMC_PFAPR_M1PFD_MASK | FMC_PFAPR_M0PFD_MASK;    
//    ///设置系统分频器
//    //MCG=PLL, core = MCG, bus = MCG/3, FlexBus = MCG/3, Flash clock= MCG/8
//    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV2(2) 
//                 | SIM_CLKDIV1_OUTDIV3(2) | SIM_CLKDIV1_OUTDIV4(7);       
//    
//    //从新存FMC_PFAPR的原始值
//    FMC_PFAPR = temp_reg; 
//    
//    //设置VCO分频器，使能PLL为100MHz, LOLIE=0, PLLS=1, CME=0, VDIV=26
//    MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV0(30);  //VDIV = 31 (x54)
//                                                  //VDIV = 26 (x50)
//    while (!(MCG_S & MCG_S_PLLST_MASK)){}; // wait for PLL status bit to set    
//    while (!(MCG_S & MCG_S_LOCK0_MASK)){}; // Wait for LOCK bit to set    
//    
//    //进入PBE模式    
//    //通过清零CLKS位来进入PEE模式
//    // CLKS=0, FRDIV=3, IREFS=0, IRCLKEN=0, IREFSTEN=0
//    MCG_C1 &= ~MCG_C1_CLKS_MASK;
//    
//    //等待时钟状态位更新
//    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3){};
//    //SIM_CLKDIV2 |= SIM_CLKDIV2_USBDIV(1);  
//    
//    //设置跟踪时钟为内核时钟
//    SIM_SOPT2 |= SIM_SOPT2_TRACECLKSEL_MASK;	
//    //在PTA6引脚上使能TRACE_CLKOU功能
//    PORTA_PCR6 = ( PORT_PCR_MUX(0x7));  
//    //使能FlexBus模块时钟
//    SIM_SCGC7 |= SIM_SCGC7_FLEXBUS_MASK;
//    //在PTA6引脚上使能FB_CLKOUT功能
//    PORTC_PCR3 = ( PORT_PCR_MUX(0x5));
//}
////锁相环频率为50/13*54=207.7M测试函数
//void pllinit207M(void)
//{
//	uint32_t temp_reg;
//        //使能IO端口时钟    
//    SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
//                              | SIM_SCGC5_PORTB_MASK
//                              | SIM_SCGC5_PORTC_MASK
//                              | SIM_SCGC5_PORTD_MASK
//                              | SIM_SCGC5_PORTE_MASK );
//    //这里处在默认的FEI模式
//    //首先移动到FBE模式
//    MCG_C2 = 0;  
//    //MCG_C2 = MCG_C2_RANGE(2) | MCG_C2_HGO_MASK | MCG_C2_EREFS_MASK;
//    //初始化晶振后释放锁定状态的振荡器和GPIO
//    SIM_SCGC4 |= SIM_SCGC4_LLWU_MASK;
////    LLWU_CS |= LLWU_CS_ACKISO_MASK;
//     LLWU_FILT1 |= LLWU_FILT1_FILTF_MASK;   
//    
//    //选择外部晶振，参考分频器，清IREFS来启动外部晶振
//    //011 If RANGE = 0, Divide Factor is 8; for all other RANGE values, Divide Factor is 256.
//    MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(3);
//    
//    //等待晶振稳定	    
//    //while (!(MCG_S & MCG_S_OSCINIT_MASK)){}              //等待锁相环初始化结束
//    while (MCG_S & MCG_S_IREFST_MASK){}                  //等待时钟切换到外部参考时钟
//    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2){}
//    
//    //进入FBE模式,
//    //0x18==25分频=2M,
//    //0x08==15分频=3.333M 
//    //0x09==16分频=3.125M,
//    //0x10==17分频=2.94M 
//    //0x11==18分频=2.7778M 
//    //0x12==19分频=2.63M,
//    //0x13==20分频=2.5M    
//    MCG_C5 = MCG_C5_PRDIV0(0x0C);                
//    
//    //确保MCG_C6处于复位状态，禁止LOLIE、PLL、和时钟控制器，清PLL VCO分频器
//    MCG_C6 = 0x0;
//    
//    //保存FMC_PFAPR当前的值
//    temp_reg = FMC_PFAPR;
//    
//    //通过M&PFD置位M0PFD来禁止预取功能
//    FMC_PFAPR |= FMC_PFAPR_M7PFD_MASK | FMC_PFAPR_M6PFD_MASK | FMC_PFAPR_M5PFD_MASK
//                     | FMC_PFAPR_M4PFD_MASK | FMC_PFAPR_M3PFD_MASK | FMC_PFAPR_M2PFD_MASK
//                     | FMC_PFAPR_M1PFD_MASK | FMC_PFAPR_M0PFD_MASK;    
//    ///设置系统分频器
//    //MCG=PLL, core = MCG, bus = MCG/3, FlexBus = MCG/3, Flash clock= MCG/8
//    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV2(2) 
//                 | SIM_CLKDIV1_OUTDIV3(2) | SIM_CLKDIV1_OUTDIV4(7);       
//    
//    //从新存FMC_PFAPR的原始值
//    FMC_PFAPR = temp_reg; 
//    
//    //设置VCO分频器，使能PLL为100MHz, LOLIE=0, PLLS=1, CME=0, VDIV=26
//    MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV0(30);  //VDIV = 31 (x54)
//                                                  //VDIV = 26 (x50)
//    while (!(MCG_S & MCG_S_PLLST_MASK)){}; // wait for PLL status bit to set    
//    while (!(MCG_S & MCG_S_LOCK0_MASK)){}; // Wait for LOCK bit to set    
//    
//    //进入PBE模式    
//    //通过清零CLKS位来进入PEE模式
//    // CLKS=0, FRDIV=3, IREFS=0, IRCLKEN=0, IREFSTEN=0
//    MCG_C1 &= ~MCG_C1_CLKS_MASK;
//    
//    //等待时钟状态位更新
//    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3){};
//    //SIM_CLKDIV2 |= SIM_CLKDIV2_USBDIV(1);  
//    
//    //设置跟踪时钟为内核时钟
//    SIM_SOPT2 |= SIM_SOPT2_TRACECLKSEL_MASK;	
//    //在PTA6引脚上使能TRACE_CLKOU功能
//    PORTA_PCR6 = ( PORT_PCR_MUX(0x7));  
//    //使能FlexBus模块时钟
//    SIM_SCGC7 |= SIM_SCGC7_FLEXBUS_MASK;
//    //在PTA6引脚上使能FB_CLKOUT功能
//    PORTC_PCR3 = ( PORT_PCR_MUX(0x5));
//}


////锁相环频率为50/16*54=168.75M测试函数
//void pllinit168d75M(void)
//{
//	uint32_t temp_reg;
//        //使能IO端口时钟    
//    SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
//                              | SIM_SCGC5_PORTB_MASK
//                              | SIM_SCGC5_PORTC_MASK
//                              | SIM_SCGC5_PORTD_MASK
//                              | SIM_SCGC5_PORTE_MASK );
//    //这里处在默认的FEI模式
//    //首先移动到FBE模式
//    MCG_C2 = 0;  
//    //MCG_C2 = MCG_C2_RANGE(2) | MCG_C2_HGO_MASK | MCG_C2_EREFS_MASK;
//    //初始化晶振后释放锁定状态的振荡器和GPIO
//    SIM_SCGC4 |= SIM_SCGC4_LLWU_MASK;
////    LLWU_CS |= LLWU_CS_ACKISO_MASK;
//     LLWU_FILT1 |= LLWU_FILT1_FILTF_MASK;   
//    
//    //选择外部晶振，参考分频器，清IREFS来启动外部晶振
//    //011 If RANGE = 0, Divide Factor is 8; for all other RANGE values, Divide Factor is 256.
//    MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(3);
//    
//    //等待晶振稳定	    
//    //while (!(MCG_S & MCG_S_OSCINIT_MASK)){}              //等待锁相环初始化结束
//    while (MCG_S & MCG_S_IREFST_MASK){}                  //等待时钟切换到外部参考时钟
//    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2){}
//    
//    //进入FBE模式,
//    //0x18==25分频=2M,
//    //0x08==15分频=3.333M 
//    //0x09==16分频=3.125M,
//    //0x10==17分频=2.94M 
//    //0x11==18分频=2.7778M 
//    //0x12==19分频=2.63M,
//    //0x13==20分频=2.5M    
//    MCG_C5 = MCG_C5_PRDIV0(0x0f);                
//    
//    //确保MCG_C6处于复位状态，禁止LOLIE、PLL、和时钟控制器，清PLL VCO分频器
//    MCG_C6 = 0x0;
//    
//    //保存FMC_PFAPR当前的值
//    temp_reg = FMC_PFAPR;
//    
//    //通过M&PFD置位M0PFD来禁止预取功能
//    FMC_PFAPR |= FMC_PFAPR_M7PFD_MASK | FMC_PFAPR_M6PFD_MASK | FMC_PFAPR_M5PFD_MASK
//                     | FMC_PFAPR_M4PFD_MASK | FMC_PFAPR_M3PFD_MASK | FMC_PFAPR_M2PFD_MASK
//                     | FMC_PFAPR_M1PFD_MASK | FMC_PFAPR_M0PFD_MASK;    
//    ///设置系统分频器
//    //MCG=PLL, core = MCG, bus = MCG/3, FlexBus = MCG/3, Flash clock= MCG/8
//    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV2(2) 
//                 | SIM_CLKDIV1_OUTDIV3(2) | SIM_CLKDIV1_OUTDIV4(7);       
//    
//    //从新存FMC_PFAPR的原始值
//    FMC_PFAPR = temp_reg; 
//    
//    //设置VCO分频器，使能PLL为100MHz, LOLIE=0, PLLS=1, CME=0, VDIV=26
//    MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV0(30);  //VDIV = 31 (x54)
//                                                  //VDIV = 26 (x50)
//    while (!(MCG_S & MCG_S_PLLST_MASK)){}; // wait for PLL status bit to set    
//    while (!(MCG_S & MCG_S_LOCK0_MASK)){}; // Wait for LOCK bit to set    
//    
//    //进入PBE模式    
//    //通过清零CLKS位来进入PEE模式
//    // CLKS=0, FRDIV=3, IREFS=0, IRCLKEN=0, IREFSTEN=0
//    MCG_C1 &= ~MCG_C1_CLKS_MASK;
//    
//    //等待时钟状态位更新
//    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3){};
//    //SIM_CLKDIV2 |= SIM_CLKDIV2_USBDIV(1);  
//    
//    //设置跟踪时钟为内核时钟
//    SIM_SOPT2 |= SIM_SOPT2_TRACECLKSEL_MASK;	
//    //在PTA6引脚上使能TRACE_CLKOU功能
//    PORTA_PCR6 = ( PORT_PCR_MUX(0x7));  
//    //使能FlexBus模块时钟
//    SIM_SCGC7 |= SIM_SCGC7_FLEXBUS_MASK;
//    //在PTA6引脚上使能FB_CLKOUT功能
//    PORTC_PCR3 = ( PORT_PCR_MUX(0x5));
//}
////锁相环频率为50/18*54=150M测试函数
//void pllinit150M(void)
//{
//	uint32_t temp_reg;
//        //使能IO端口时钟    
//    SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
//                              | SIM_SCGC5_PORTB_MASK
//                              | SIM_SCGC5_PORTC_MASK
//                              | SIM_SCGC5_PORTD_MASK
//                              | SIM_SCGC5_PORTE_MASK );
//    //这里处在默认的FEI模式
//    //首先移动到FBE模式
//    MCG_C2 = 0;  
//    //MCG_C2 = MCG_C2_RANGE(2) | MCG_C2_HGO_MASK | MCG_C2_EREFS_MASK;
//    //初始化晶振后释放锁定状态的振荡器和GPIO
//    SIM_SCGC4 |= SIM_SCGC4_LLWU_MASK;
////    LLWU_CS |= LLWU_CS_ACKISO_MASK;
//     LLWU_FILT1 |= LLWU_FILT1_FILTF_MASK;   
//    
//    //选择外部晶振，参考分频器，清IREFS来启动外部晶振
//    //011 If RANGE = 0, Divide Factor is 8; for all other RANGE values, Divide Factor is 256.
//    MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(3);
//    
//    //等待晶振稳定	    
//    //while (!(MCG_S & MCG_S_OSCINIT_MASK)){}              //等待锁相环初始化结束
//    while (MCG_S & MCG_S_IREFST_MASK){}                  //等待时钟切换到外部参考时钟
//    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2){}
//    
//    //进入FBE模式,
//    //0x18==25分频=2M,
//    //0x11==18分频=2.7778M 
//    //0x12==19分频=2.63M,
//    //0x13==20分频=2.5M    
//    MCG_C5 = MCG_C5_PRDIV0(0x11);                
//    
//    //确保MCG_C6处于复位状态，禁止LOLIE、PLL、和时钟控制器，清PLL VCO分频器
//    MCG_C6 = 0x0;
//    
//    //保存FMC_PFAPR当前的值
//    temp_reg = FMC_PFAPR;
//    
//    //通过M&PFD置位M0PFD来禁止预取功能
//    FMC_PFAPR |= FMC_PFAPR_M7PFD_MASK | FMC_PFAPR_M6PFD_MASK | FMC_PFAPR_M5PFD_MASK
//                     | FMC_PFAPR_M4PFD_MASK | FMC_PFAPR_M3PFD_MASK | FMC_PFAPR_M2PFD_MASK
//                     | FMC_PFAPR_M1PFD_MASK | FMC_PFAPR_M0PFD_MASK;    
//    ///设置系统分频器
//    //MCG=PLL, core = MCG, bus = MCG/2, FlexBus = MCG/2, Flash clock= MCG/8
//    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV2(1) 
//                 | SIM_CLKDIV1_OUTDIV3(1) | SIM_CLKDIV1_OUTDIV4(7);       
//    
//    //从新存FMC_PFAPR的原始值
//    FMC_PFAPR = temp_reg; 
//    
//    //设置VCO分频器，使能PLL为100MHz, LOLIE=0, PLLS=1, CME=0, VDIV=26
//    MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV0(30);  //VDIV = 31 (x54)
//                                                  //VDIV = 26 (x50)
//    while (!(MCG_S & MCG_S_PLLST_MASK)){}; // wait for PLL status bit to set    
//    while (!(MCG_S & MCG_S_LOCK0_MASK)){}; // Wait for LOCK bit to set    
//    
//    //进入PBE模式    
//    //通过清零CLKS位来进入PEE模式
//    // CLKS=0, FRDIV=3, IREFS=0, IRCLKEN=0, IREFSTEN=0
//    MCG_C1 &= ~MCG_C1_CLKS_MASK;
//    
//    //等待时钟状态位更新
//    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3){};
//    //SIM_CLKDIV2 |= SIM_CLKDIV2_USBDIV(1);  
//    
//    //设置跟踪时钟为内核时钟
//    SIM_SOPT2 |= SIM_SOPT2_TRACECLKSEL_MASK;	
//    //在PTA6引脚上使能TRACE_CLKOU功能
//    PORTA_PCR6 = ( PORT_PCR_MUX(0x7));  
//    //使能FlexBus模块时钟
//    SIM_SCGC7 |= SIM_SCGC7_FLEXBUS_MASK;
//    //在PTA6引脚上使能FB_CLKOUT功能
//    PORTC_PCR3 = ( PORT_PCR_MUX(0x5));
//}
////锁相环频率为50/19*55=144.7386421M测试函数
//void pllinit144M(void)
//{
//	uint32_t temp_reg;
//        //使能IO端口时钟    
//    SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
//                              | SIM_SCGC5_PORTB_MASK
//                              | SIM_SCGC5_PORTC_MASK
//                              | SIM_SCGC5_PORTD_MASK
//                              | SIM_SCGC5_PORTE_MASK );
//    //这里处在默认的FEI模式
//    //首先移动到FBE模式
//    MCG_C2 = 0;  
//    //MCG_C2 = MCG_C2_RANGE(2) | MCG_C2_HGO_MASK | MCG_C2_EREFS_MASK;
//    //初始化晶振后释放锁定状态的振荡器和GPIO
//    SIM_SCGC4 |= SIM_SCGC4_LLWU_MASK;
////    LLWU_CS |= LLWU_CS_ACKISO_MASK;
//     LLWU_FILT1 |= LLWU_FILT1_FILTF_MASK;   
//    
//    //选择外部晶振，参考分频器，清IREFS来启动外部晶振
//    //011 If RANGE = 0, Divide Factor is 8; for all other RANGE values, Divide Factor is 256.
//    MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(3);
//    
//    //等待晶振稳定	    
//    //while (!(MCG_S & MCG_S_OSCINIT_MASK)){}              //等待锁相环初始化结束
//    while (MCG_S & MCG_S_IREFST_MASK){}                  //等待时钟切换到外部参考时钟
//    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2){}
//    
//    //进入FBE模式,
//    //0x18==25分频=2M,
//    //0x11==18分频=2.7778M 
//    //0x12==19分频=2.63M,
//    //0x13==20分频=2.5M    
//    MCG_C5 = MCG_C5_PRDIV0(0x12);                
//    
//    //确保MCG_C6处于复位状态，禁止LOLIE、PLL、和时钟控制器，清PLL VCO分频器
//    MCG_C6 = 0x0;
//    
//    //保存FMC_PFAPR当前的值
//    temp_reg = FMC_PFAPR;
//    
//    //通过M&PFD置位M0PFD来禁止预取功能
//    FMC_PFAPR |= FMC_PFAPR_M7PFD_MASK | FMC_PFAPR_M6PFD_MASK | FMC_PFAPR_M5PFD_MASK
//                     | FMC_PFAPR_M4PFD_MASK | FMC_PFAPR_M3PFD_MASK | FMC_PFAPR_M2PFD_MASK
//                     | FMC_PFAPR_M1PFD_MASK | FMC_PFAPR_M0PFD_MASK;    
//    ///设置系统分频器
//    //MCG=PLL, core = MCG, bus = MCG/2, FlexBus = MCG/2, Flash clock= MCG/6
//    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0) | SIM_CLKDIV1_OUTDIV2(1) 
//                 | SIM_CLKDIV1_OUTDIV3(1) | SIM_CLKDIV1_OUTDIV4(5);       
//    
//    //从新存FMC_PFAPR的原始值
//    FMC_PFAPR = temp_reg; 
//    
//    //设置VCO分频器，使能PLL为100MHz, LOLIE=0, PLLS=1, CME=0, VDIV=26
//    MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV0(31);  //VDIV = 31 (x55)
//                                                  //VDIV = 26 (x50)
//    while (!(MCG_S & MCG_S_PLLST_MASK)){}; // wait for PLL status bit to set    
//    while (!(MCG_S & MCG_S_LOCK0_MASK)){}; // Wait for LOCK bit to set    
//    
//    //进入PBE模式    
//    //通过清零CLKS位来进入PEE模式
//    // CLKS=0, FRDIV=3, IREFS=0, IRCLKEN=0, IREFSTEN=0
//    MCG_C1 &= ~MCG_C1_CLKS_MASK;
//    
//    //等待时钟状态位更新
//    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3){};
//    //SIM_CLKDIV2 |= SIM_CLKDIV2_USBDIV(1);  
//    
//    //设置跟踪时钟为内核时钟
//    SIM_SOPT2 |= SIM_SOPT2_TRACECLKSEL_MASK;	
//    //在PTA6引脚上使能TRACE_CLKOU功能
//    PORTA_PCR6 = ( PORT_PCR_MUX(0x7));  
//    //使能FlexBus模块时钟
//    SIM_SCGC7 |= SIM_SCGC7_FLEXBUS_MASK;
//    //在PTA6引脚上使能FB_CLKOUT功能
//    PORTC_PCR3 = ( PORT_PCR_MUX(0x5));
//}



