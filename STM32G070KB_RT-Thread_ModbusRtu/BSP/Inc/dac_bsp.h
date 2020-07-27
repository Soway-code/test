/**@file        dac_bsp.h
* @details      dac_bsp.c的头文件,声明了DAC的API函数
* @author       杨春林
* @date         2020-04-27
* @version      V1.0.0
* @copyright    2020-2030,深圳市信为科技发展有限公司
**********************************************************************************
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/04/27  <td>1.0.0    <td>杨春林    <td>创建初始版本
* </table>
*
**********************************************************************************
*/

#ifndef __dac_H
#define __dac_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"


#define DACx                            DAC
#define DACx_CHANNEL_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()

#define DACx_CLK_ENABLE()               __HAL_RCC_DAC1_CLK_ENABLE()
#define DACx_CLK_DISABLE()              __HAL_RCC_DAC1_CLK_DISABLE()

/* Definition for DACx Channel Pin */
#define DACx_CHANNEL1_PIN               GPIO_PIN_4
#define DACx_CHANNEL1_GPIO_PORT         GPIOA

#define DACx_CHANNEL2_PIN               GPIO_PIN_5
#define DACx_CHANNEL2_GPIO_PORT         GPIOA


//#define DAC_CHANNEL_1_ENABLE
#define DAC_CHANNEL_2_ENABLE

typedef enum {
    DA_CHANNEL_1,
    DA_CHANNEL_2,
    DA_CHANNEL_TOTAL
}DA_CHANNEL_NUM;

/* 使用RT-Thread操作系统,USING_RT_THREAD_OS在main.h中定义 */
#ifdef USING_RT_THREAD_OS
#include "board.h"

#define DAC_DEVICE_NAME     "dac"

#endif


/**@brief       DAC初始化
* @return       函数执行结果
* - None
* @note         DAC通道通过DAC_CHANNEL_1_ENABLE和DAC_CHANNEL_2_ENABLE宏定义打开
*/
void BSP_DAC_Init(void);

/**@brief       指定DAC通道写入DA值
* @param[in]    DA_Value : DAC值
* @param[in]    DA_Channel_Num : DAC通道号
* @return       函数执行结果
* - None
* @note         DA值不能超过4095; 通道号见DA_CHANNEL_NUM的定义,
* 其值只能取DA_CHANNEL_NUM定义的值
*/
uint8_t BSP_DAC_Convert(uint16_t DA_Value, DA_CHANNEL_NUM DA_Channel_Num);

#ifdef __cplusplus
}
#endif
#endif /*__ dac_H */
