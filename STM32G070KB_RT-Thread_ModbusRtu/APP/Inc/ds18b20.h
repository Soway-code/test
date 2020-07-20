#ifndef __DS18B20_H
#define __DS18B20_H
#include "main.h"
#include "board.h"
#include "rtthread.h"

#define RCC_GPIOX_CLK 		__HAL_RCC_GPIOA_CLK_ENABLE();						///����GPIOBʱ��
#define DS18B20_GPIOX 		GPIOA
#define DS18B20_GPIO_PINX 	GPIO_PIN_15

////IO��������											   
#define	DS18B20_DQ_IN  HAL_GPIO_ReadPin(DS18B20_GPIOX,DS18B20_GPIO_PINX)  //���ݶ˿�	PB0 
 
uint8_t DS18B20_Init(void);				//��ʼ��DS18B20
short DS18B20_Get_Temp(void);			//��ȡ�¶�
void DS18B20_Start(void);				//��ʼ�¶�ת��
void DS18B20_Write_Byte(uint8_t dat);	//д��һ���ֽ�
uint8_t DS18B20_Read_Byte(void);		//����һ���ֽ�
uint8_t DS18B20_Read_Bit(void);			//����һ��λ
uint8_t DS18B20_Check(void);			//����Ƿ����DS18B20
void DS18B20_Rst(void);					//��λDS18B20 
#endif

