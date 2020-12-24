/*--------------*/
/*-----FF.H-----*/
/*--------------*/
/*-----INC------*/
/*--------------*/
#ifndef FF_H
#define FF_H


#include "adc.h"
#include "arm_math.h"
#include "at24cxx.h"
#include "common.h"
#include "crc.h"
#include "flexbus.h"
#include "gpio.h"
#include "gpio64.h"
#include "i2c.h"
#include "includes64.h"
#include "IS61WV25616.h" 
#include "LQ12864.h"
#include "MK64F12.h"
#include "pit.h"
#include "sd.h"
#include <stdint.h>
#include "stdio.h"
#include "systick.h"
#include "uart.h"
#include "wdog.h"

/*-----------------------------------------------------------------------------------------------------*/
#define SIM_SCGC4_LLWU_MASK   0x10000000u
/*-----------------------------------------------------------------------------------------------------*/
/* Num of Samples per Cycle */
#define dotnum 4096
/*-----------------------------------------------------------------------------------------------------*/
/* LED Define */
#define LED_RUN PDout(14)
#define LED_INI PDout(15)
/*-----------------------------------------------------------------------------------------------------*/
/* ADC Define *///EDITED.2018.10.30
#define BUSY PAin(26)
#define Dout_A PAin(24)
#define Dout_B PAin(25)

#define ADDR PAout(19) //0
#define CS PAout(29)
#define SCLK PAout(28)
#define CNVST PAout(27)

#define RANGE0 PAout(16)
#define RANGE1 PAout(17)
/*-----------------------------------------------------------------------------------------------------*/
/* Press Read ADC Define */
#define Press_ADC ADC0_SE1_DP1
#define TEMP_ADC ADC0_SE20_DM1
/*-----------------------------------------------------------------------------------------------------*/
/* DCP Define */
#define INC PAout(8)
#define UD PAout(9)
#define RES1 PAout(10)
#define RES2 PAout(11)
/*-----------------------------------------------------------------------------------------------------*/
/* Thermocouple SPI Define */
#define T_SCK PAout(26)
#define T_CS PAout(27)
#define T_SO PAout(28)
/*-----------------------------------------------------------------------------------------------------*/
/* Serial Ports Define */
#define SC_0 UART5_RX_PE09_TX_PE08  //UART5 to-Host
#define SC_0_R_W PEout(7)

#define SC_1 UART4_RX_PE25_TX_PE24  //UART4 to-Slave
#define SC_1_R_W PEout(26)

#define SC_2 UART3_RX_PE05_TX_PE04  //UART3 to-GPRS
/*-----------------------------------------------------------------------------------------------------*/
/* EEPROM I2C Define */
#define EEPROM_I2C I2C0_SCL_PB02_SDA_PB03
/*-----------------------------------------------------------------------------------------------------*/
#define PAGE_SIZE       (8)

/*-----------------------------------------------------------------------------------------------------*/
/* Local Host Addr */
static uint8_t device_addr=0x01;//本机地址
/*-----------------------------------------------------------------------------------------------------*/

//L
//上下游传感器间距
#define L 250
//#define alpha 0.1798
//0.2009@40Hz
//0.2238@50Hz
#define dm_air 1.1494
//L

/*-----------------------------------------------------------------------------------------------------*/
/* Parallel Communication */
//out
#define FB_D0 PCout(0)
#define FB_D1 PCout(1)
#define FB_D2 PCout(2)
#define FB_D3 PCout(3)
#define FB_D4 PCout(4)
#define FB_D5 PCout(5)
#define FB_D6 PCout(6)
#define FB_D7 PCout(7)

#define FB_D8 PCout(8)
#define FB_D9 PCout(9)
#define FB_D10 PCout(10)
#define FB_D11 PCout(11)
#define FB_D12 PCout(12)
#define FB_D13 PCout(13)
#define FB_D14 PCout(14)
#define FB_D15 PCout(15)

#define FB_B0 PDout(0)
#define FB_B1 PDout(4)
#define FB_B2 PDout(5)

//in
#define FB_CS0 PCin(19)
#define FB_CS1 PDin(1)

#define FB_A0 PDin(2)
#define FB_A1 PDin(3)

#define SPI_CS0 PCin(0)
#define SPI_CLK0 PCin(1)
#define SPI_DO0 PCout(2)

#define SPI_CS1 PCin(3)
#define SPI_CLK1 PCin(4)
#define SPI_DO1 PCout(5)

#define SPI_CS2 PCin(6)
#define SPI_CLK2 PCin(7)
#define SPI_DO2 PCout(8)
/*-----------------------------------------------------------------------------------------------------*/
#define slaveAddr 2

#define bScale 5
#define cScale 25

#endif
