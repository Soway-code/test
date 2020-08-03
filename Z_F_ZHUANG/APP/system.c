/**
* @file         system.c
* @details      ϵͳ��ʼ��
* @author       weijianx
* @date         2020-07-02
* @version      V1.0.0
* @copyright    2020-2030,��������Ϊ�Ƽ���չ���޹�˾
**********************************************************************************
* @par �޸���־:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/07/02  <td>1.0.0    <td>weijianx  <td>������ʼ�汾
* </table>
*
**********************************************************************************
*/

#include "system.h"
#include "config.h"


/**
* @brief        ϵͳ��ʼ��
* @param        None
* @return       None
*/
void system_Init(void)
{
//	RemapInterruptVerctor();
	HAL_Init();
	
	SystemClock_Config();

	MX_GPIO_Init();
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	MX_DMA_Init();
	MX_ADC1_Init();
	MX_USART1_UART_Init();
	MX_TIM1_Init();
	MX_TIM3_Init();
	User_Iwdg_Init();

}



