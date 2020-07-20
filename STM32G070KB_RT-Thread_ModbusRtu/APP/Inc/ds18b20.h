#ifndef __DS18B20_H
#define __DS18B20_H
#include "main.h"
#include "board.h"
#include "rtthread.h"

#define RCC_GPIOX_CLK 		__HAL_RCC_GPIOA_CLK_ENABLE();						///开启GPIOB时钟
#define DS18B20_GPIOX 		GPIOA
#define DS18B20_GPIO_PINX 	GPIO_PIN_15

////IO操作函数											   
#define	DS18B20_DQ_IN  HAL_GPIO_ReadPin(DS18B20_GPIOX,DS18B20_GPIO_PINX)  //数据端口	PB0 
 
uint8_t DS18B20_Init(void);				//初始化DS18B20
short DS18B20_Get_Temp(void);			//获取温度
void DS18B20_Start(void);				//开始温度转换
void DS18B20_Write_Byte(uint8_t dat);	//写入一个字节
uint8_t DS18B20_Read_Byte(void);		//读出一个字节
uint8_t DS18B20_Read_Bit(void);			//读出一个位
uint8_t DS18B20_Check(void);			//检测是否存在DS18B20
void DS18B20_Rst(void);					//复位DS18B20 
#endif

