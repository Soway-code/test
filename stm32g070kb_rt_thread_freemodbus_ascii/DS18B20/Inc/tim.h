/**@file        tim.h
* @details      tim.c��ͷ�ļ�,�����˶�ʱ��ѡ��ĺ궨��,������timӦ�õ�API����
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
* @brief ��ȡHCLKʱ��Ƶ��
* @return ����ֵ��HCLKʱ��Ƶ��
* @retval ��ȡHCLKʱ��Ƶ��ʧ�ܷ��� 0
*/
uint8_t Get_HCLKFreq(void);	

/**
* @brief ��ʼ����ʱ��
*/
void MX_TIMx_Init(void);

/**
* @brief 1΢����ʱ����
*/
void delay_tim_1us(void);

/**
* @brief 2΢����ʱ����
*/
void delay_tim_2us(void);

/**
* @brief 5΢����ʱ����
*/
void delay_tim_5us(void);

/**
* @brief 15΢����ʱ����
*/
void delay_tim_15us(void);

/**
* @brief 60΢����ʱ����
*/
void delay_tim_60us(void);

/**
* @brief 750΢����ʱ����
*/
void delay_tim_750us(void);

/**
* @brief ������ʱ����
* @param[in]   ��Ҫ��ʱ��ʱ��(ms)
*/
void delay_tim_ms(uint32_t ms);

/**
* @brief ����ʱ����
* @param[in]   ��Ҫ��ʱ��ʱ��(s)
*/
void delay_tim_s(uint32_t s);

#ifdef __cplusplus
}
#endif
#endif 
