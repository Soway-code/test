/**@file        tim.h
* @details      tim.c的头文件,定义了定时器选择的宏定义,声明了tim应用的API函数
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
#ifndef __tim_H
#define __tim_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "ds18b20.h"


#if USE_TIMER_X != 16
	extern TIM_HandleTypeDef htim3;
#else
	extern TIM_HandleTypeDef htim14;
#endif


/**
* @brief 获取HCLK时钟频率
* @return 返回值：HCLK时钟频率
* @retval 获取HCLK时钟频率失败返回 0
*/
uint8_t Get_HCLKFreq(void);	

/**
* @brief 初始化定时器
*/
void MX_TIMx_Init(void);

/**
* @brief 1微秒延时函数
*/
void delay_tim_1us(void);

/**
* @brief 2微秒延时函数
*/
void delay_tim_2us(void);

/**
* @brief 5微秒延时函数
*/
void delay_tim_5us(void);

/**
* @brief 15微秒延时函数
*/
void delay_tim_15us(void);

/**
* @brief 60微秒延时函数
*/
void delay_tim_60us(void);

/**
* @brief 750微秒延时函数
*/
void delay_tim_750us(void);

/**
* @brief 毫秒延时函数
* @param[in]   需要延时的时间(ms)
*/
void delay_tim_ms(uint32_t ms);

/**
* @brief 秒延时函数
* @param[in]   需要延时的时间(s)
*/
void delay_tim_s(uint32_t s);

#ifdef __cplusplus
}
#endif
#endif 
