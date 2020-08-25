/**@file        ds18b20.h
* @details      ds18b20.c��ͷ�ļ�,����������ds18b20�ܽźͶ�ʱ��Ӧ�õĺ궨��,������ds18b20Ӧ�õ�API����
* @author       �����
* @date         2020-08-01
* @version      V1.0.0
* @copyright    2020-2030,��������Ϊ�Ƽ���չ���޹�˾
**********************************************************************************
* @par �޸���־:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/08/01  <td>1.0.0    <td>�����    <td>������ʼ�汾
* </table>
*
**********************************************************************************
*/

#ifndef __DS18B20_H
#define __DS18B20_H
/**
*@brief main.h
*main.hͷ�ļ��������ͷ�ļ��� #include "stm32f4xx_hal.h"
*�������ʹ��STM32CubeMX������ɵĹ���û�� main.h ����ע��
* main.h ����϶�Ӧ�� #include "stm32(xx)xx_hal.h"
*/
#include "main.h"	
#include "tim.h"

/**
*@brief RCC_GPIOX_CLK 
*RCC_GPIOX_CLK 		//����GPIOxʱ��
*DS18B20_GPIOX 		//ѡ��GPIOx�˿�
*DS18B20_GPIO_PINX	//ѡ��ܽ�
*/
#define RCC_GPIOX_CLK 		__HAL_RCC_GPIOA_CLK_ENABLE();		//����GPIOxʱ��
#define DS18B20_GPIOX 		GPIOA								//ѡ��DS18B20 GPIOx�˿�
#define DS18B20_GPIO_PINX 	GPIO_PIN_15							//ѡ��DS18B20 ���ӵĹܽ�

/**
��Ҫ��Ӷ�ʱ�����ļ�stm32(xx)xx_hal_tim.c��stm32(xx)xx_hal_tim_ex.c
��Ҫ��stm32(xx)xx_hal_conf.h�ļ���� #define HAL_TIM_MODULE_ENABLED 
��ʱ������궨��ע��ȥ���������������� #include "ds18b20.h"�ȿ���ʹ�����½ӿں�����
uint8_t DS18B20_Init(void);	
short DS18B20_Get_Temp(void);
void delay_tim_ms(uint32_t ms);
void delay_tim_s(uint32_t s);
*/


/**
*@brief HclkFrequency
*HclkFrequencyĬ������Ϊ0�Զ���ȡHCLK��Ƶ���ṩ����ʱ����Ƶ		֧��24M/32M/48M/64M/72M/168M
*��ȡ�����¶�ֵʱ�����޸�HclkFrequencyΪ��Ӧ��ʱ��Ƶ��			24/32/48/64/72/168
*STM32LXX ϵ��32MƵ��ʱ��Ҫ�޸� HclkFrequency Ϊ 24
*/
#define HclkFrequency 	0			

/**
*@brief USE_TIMER_X
*USE_TIMER_XĬ������Ϊ	3	ʹ�ö�ʱ��3��	
*USE_TIMER_XΪ 			14 	ʱʹ�ö�ʱ��14����14ʹ�ö�ʱ��3��
*/
#define USE_TIMER_X     3

/**
*@brief USING_RT_THREAD_OS
*USING_RT_THREAD_OS Ĭ������Ϊ 	0	��ʹ�ò���ϵͳ��	
*USING_RT_THREAD_OS Ϊ 		   	1 	ʹ��RT-Thread����ϵͳ��
*/
#define USING_RT_THREAD_OS		0

/**
* @brief uint8_t DS18B20_Init(void);
*
* @return 0x10,1,0 
* @retval ����0x10��ʱ��Ƶ��ƥ��ʧ�ܣ�����1 DS18B20������, ����0 DS18B20���ڳ�ʼ���ɹ�
*/
uint8_t DS18B20_Init(void);	

/**
* @brief short DS18B20_Get_Temp(void);
*
* @return ����ֵ����ȡ�����¶��¶�ֵ(-550~1250)
* @retval ����ֵ���¶ȵ�ʮ�� (24.5��ʱ����245)	
*/
short DS18B20_Get_Temp(void);

#endif

