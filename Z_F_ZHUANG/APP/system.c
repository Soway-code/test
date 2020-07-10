/**
* @file         system.c
* @details      系统初始化
* @author       weijianx
* @date         2020-07-02
* @version      V1.0.0
* @copyright    2020-2030,深圳市信为科技发展有限公司
**********************************************************************************
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/07/02  <td>1.0.0    <td>weijianx  <td>创建初始版本
* </table>
*
**********************************************************************************
*/

#include "system.h"
#include "config.h"


/**
* @brief        系统初始化
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



