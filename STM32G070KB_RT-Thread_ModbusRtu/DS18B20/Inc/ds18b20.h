/**@file        ds18b20.h
* @details      ds18b20.c��ͷ�ļ�,����������ds18b20�ܽźͶ�ʱ��Ӧ�õĺ궨��,������ds18b20Ӧ�õ�API����
* @author       �����
* @date         2020-07-17
* @version      V1.0.0
* @copyright    2020-2030,��������Ϊ�Ƽ���չ���޹�˾
**********************************************************************************
* @par �޸���־:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/07/17  <td>1.0.0    <td>�����    <td>������ʼ�汾
* </table>
*
**********************************************************************************
*/

#ifndef __DS18B20_H
#define __DS18B20_H
#include "main.h"
#include "stm32g0xx_hal.h"
#include "board.h"
#include "rtthread.h"
#include "tim.h"

/**
*@brief RCC_GPIOX_CLK 
*RCC_GPIOX_CLK 		//����GPIOxʱ��
*DS18B20_GPIOX 		//ѡ��GPIOx�˿�
*DS18B20_GPIO_PINX	//ѡ��ܽ�
*/
#define RCC_GPIOX_CLK 		__HAL_RCC_GPIOA_CLK_ENABLE();						///����GPIOBʱ��
#define DS18B20_GPIOX 		GPIOA
#define DS18B20_GPIO_PINX 	GPIO_PIN_15

////IO��������											   
#define	DS18B20_DQ_IN  HAL_GPIO_ReadPin(DS18B20_GPIOX,DS18B20_GPIO_PINX)  //���ݶ˿�	PB0 
 
/**
* @brief uint8_t DS18B20_Init(void);
*
* @return 1,0 
* @retval ����1 DS18B20������, ����0 DS18B20���ڳ�ʼ���ɹ�
*/
uint8_t DS18B20_Init(void);	

/**
* @brief short DS18B20_Get_Temp(void);
*
* @return ����ֵ����ȡ�����¶��¶�ֵ(-550~1250)
* @retval ����ֵ���¶ȵ�ʮ�� (24.5��ʱ����245)	
*/
short DS18B20_Get_Temp(void);

/**
* @brief ����Ƿ����DS18B20
*/
uint8_t DS18B20_Check(void);

/**
* @brief ��λDS18B20
*/
void DS18B20_Rst(void);	

#endif

