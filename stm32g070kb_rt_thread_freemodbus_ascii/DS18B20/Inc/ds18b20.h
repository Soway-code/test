/**@file        ds18b20.h
* @details      ds18b20.c的头文件,定义了配置ds18b20管脚和定时器应用的宏定义,声明了ds18b20应用的API函数
* @author       马灿林
* @date         2020-08-01
* @version      V1.0.0
* @copyright    2020-2030,深圳市信为科技发展有限公司
**********************************************************************************
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/08/01  <td>1.0.0    <td>马灿林    <td>创建初始版本
* </table>
*
**********************************************************************************
*/

#ifndef __DS18B20_H
#define __DS18B20_H
/**
*@brief main.h
*main.h头文件里包含的头文件是 #include "stm32f4xx_hal.h"
*如果不是使用STM32CubeMX软件生成的工程没有 main.h 可以注释
* main.h 添加上对应的 #include "stm32(xx)xx_hal.h"
*/
#include "main.h"	
#include "tim.h"

/**
*@brief RCC_GPIOX_CLK 
*RCC_GPIOX_CLK 		//开启GPIOx时钟
*DS18B20_GPIOX 		//选择GPIOx端口
*DS18B20_GPIO_PINX	//选择管脚
*/
#define RCC_GPIOX_CLK 		__HAL_RCC_GPIOA_CLK_ENABLE();		//开启GPIOx时钟
#define DS18B20_GPIOX 		GPIOA								//选择DS18B20 GPIOx端口
#define DS18B20_GPIO_PINX 	GPIO_PIN_15							//选择DS18B20 连接的管脚

/**
需要添加定时器库文件stm32(xx)xx_hal_tim.c和stm32(xx)xx_hal_tim_ex.c
需要在stm32(xx)xx_hal_conf.h文件里把 #define HAL_TIM_MODULE_ENABLED 
定时器这个宏定义注释去掉，在主函数引用 #include "ds18b20.h"既可以使用以下接口函数。
uint8_t DS18B20_Init(void);	
short DS18B20_Get_Temp(void);
void delay_tim_ms(uint32_t ms);
void delay_tim_s(uint32_t s);
*/


/**
*@brief HclkFrequency
*HclkFrequency默认设置为0自动获取HCLK的频率提供给定时器分频		支持24M/32M/48M/64M/72M/168M
*获取不到温度值时可以修改HclkFrequency为对应的时钟频率			24/32/48/64/72/168
*STM32LXX 系列32M频率时需要修改 HclkFrequency 为 24
*/
#define HclkFrequency 	0			

/**
*@brief USE_TIMER_X
*USE_TIMER_X默认设置为	3	使用定时器3；	
*USE_TIMER_X为 			14 	时使用定时器14，非14使用定时器3，
*/
#define USE_TIMER_X     3

/**
*@brief USING_RT_THREAD_OS
*USING_RT_THREAD_OS 默认设置为 	0	不使用操作系统；	
*USING_RT_THREAD_OS 为 		   	1 	使用RT-Thread操作系统；
*/
#define USING_RT_THREAD_OS		0

/**
* @brief uint8_t DS18B20_Init(void);
*
* @return 0x10,1,0 
* @retval 返回0x10定时器频率匹配失败，返回1 DS18B20不存在, 返回0 DS18B20存在初始化成功
*/
uint8_t DS18B20_Init(void);	

/**
* @brief short DS18B20_Get_Temp(void);
*
* @return 返回值：获取到的温度温度值(-550~1250)
* @retval 返回值是温度的十倍 (24.5℃时返回245)	
*/
short DS18B20_Get_Temp(void);

#endif

