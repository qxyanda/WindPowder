#ifndef __LCD_H
#define __LCD_H		

#include "stdint.h"
/* 
代码测试环境:单片机STC12LE5A60S2,晶振33M  单片机工作电压3.3V
程序默认IO连接方式：
控制线：RS-P3^5;    WR-P3^6;   RD-P3^7;   CS-P1^0;   REST-P1^2;
数据线: DB0-DB7依次连接P0^0-P0^7;  DB8-DB15依次连接P2^0-P2^7;（8位模式下DB0-DB7可以不连接）
触摸功能连接方式：(不使用触摸可不连接)
T_CLK-P1^7;  T_CS-P1^4;  T_DIN-P3^0;  T_OUT-P3^1;  T_IRQ-P3^4;
*/	
 //定义LCD的尺寸	
#define LCD_W 240
#define LCD_H 320

#define Bus_16        //16位数据模式,如果使用8位模式，请注释此语句，如果使用16位模式，请打开此句,修改8位模式之前，请确认你手里的模块是否是8位总线接口
//IO连接
//#define  LCD_DataPortH P2     //高8位数据口,8位模式下只使用高8位 
//#define  LCD_DataPortL P0     //低8位数据口	,8位模式下低8位可以不接线,请确认P0口已经上拉10K电阻,不宜太小，最小4.7K,推荐10K.

//sbit LCD_RS = P3^5;  		 //数据/命令切换
//sbit LCD_WR = P3^6;		  //写控制
//sbit LCD_RD =P3^7;		     //读控制
//sbit LCD_CS=P1^0;		//片选	
//sbit LCD_REST = P1^2;	      //复位   

extern  uint16_t BACK_COLOR, POINT_COLOR;   //背景色，画笔色

void Lcd_Init(void); 
void LCD_Clear(uint16_t Color);
void Address_set(uint32_t x1,uint32_t y1,uint32_t x2,uint32_t y2);
void LCD_WR_DATA8(char VH,char VL); //发送数据-8位参数
void LCD_WR_DATA(int da);
void LCD_WR_REG(int da);

void LCD_DrawPoint(uint16_t x,uint16_t y);//画点
void LCD_DrawPoint_big(uint16_t x,uint16_t y);//画一个大点
uint16_t  LCD_ReadPoint(uint16_t x,uint16_t y); //读点
void Draw_Circle(uint16_t x0,uint16_t y0,uint8_t r);
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);		   
void LCD_Fill(uint16_t xsta,uint16_t ysta,uint16_t xend,uint16_t yend,uint16_t color);
void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num,uint8_t mode);//显示一个字符
void LCD_ShowNum(uint16_t x,uint16_t y,uint32_t num,uint8_t len);//显示数字
void LCD_Show2Num(uint16_t x,uint16_t y,uint16_t num,uint8_t len);//显示2个数字
void LCD_ShowString(uint16_t x,uint16_t y,const uint8_t *p);		 //显示一个字符串,16字体
 
void showhanzi(uint32_t x,uint32_t y,uint32_t index);



//画笔颜色
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	  
#define BLUE         	 0x001F  
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //棕色
#define BRRED 			 0XFC07 //棕红色
#define GRAY  			 0X8430 //灰色
//GUI颜色

#define DARKBLUE      	 0X01CF	//深蓝色
#define LIGHTBLUE      	 0X7D7C	//浅蓝色  
#define GRAYBLUE       	 0X5458 //灰蓝色
//以上三色为PANEL的颜色 
 
#define LIGHTGREEN     	 0X841F //浅绿色
#define LGRAY 			 0XC618 //浅灰色(PANNEL),窗体背景色

#define LGRAYBLUE        0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE           0X2B12 //浅棕蓝色(选择条目的反色)


					  		 
#endif  
	 
	 



