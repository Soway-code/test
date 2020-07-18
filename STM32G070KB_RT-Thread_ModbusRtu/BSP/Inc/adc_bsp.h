/**@file        adc_bsp.h
* @details      adc_bsp.c的头文件,声明了ADC的API函数
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

#ifndef __ADC_BSP_H
#define __ADC_BSP_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"


//#define ADC_CHANNEL_0_ENABLE
//#define ADC_CHANNEL_1_ENABLE
#define ADC_CHANNEL_2_ENABLE
//#define ADC_CHANNEL_3_ENABLE
//#define ADC_CHANNEL_6_ENABLE
//#define ADC_CHANNEL_7_ENABLE
//#define ADC_CHANNEL_8_ENABLE
//#define ADC_CHANNEL_9_ENABLE
#define ADC_CHANNEL_TEMPSENSOR_ENABLE

typedef enum {
#ifdef ADC_CHANNEL_0_ENABLE
    AD_CHANNEL_0,
#endif
#ifdef ADC_CHANNEL_1_ENABLE
    AD_CHANNEL_1,
#endif
#ifdef ADC_CHANNEL_2_ENABLE
    AD_CHANNEL_2,
#endif
#ifdef ADC_CHANNEL_3_ENABLE
    AD_CHANNEL_3,
#endif
#ifdef ADC_CHANNEL_6_ENABLE
    AD_CHANNEL_6,
#endif
#ifdef ADC_CHANNEL_7_ENABLE
    AD_CHANNEL_7,
#endif
#ifdef ADC_CHANNEL_8_ENABLE
    AD_CHANNEL_8,
#endif
#ifdef ADC_CHANNEL_9_ENABLE
    AD_CHANNEL_9,
#endif
#ifdef ADC_CHANNEL_TEMPSENSOR_ENABLE
    AD_CHANNEL_TEMPSENSOR,
#endif
    AD_CHANNEL_TOTAL
}AD_CHANNEL_NUM;

/* Definition for ADCx clock resources */
#define ADCx                            ADC1
#define ADCx_CLK_ENABLE()               __HAL_RCC_ADC1_CLK_ENABLE()

#define DMAx_CHANNELx_CLK_ENABLE()      __HAL_RCC_DMA1_CLK_ENABLE()

#define ADCx_CLK_DISABLE()              __HAL_RCC_ADC1_CLK_DISABLE()

/* Definition for ADCx's DMA */
#define ADCx_DMA_STREAM                 DMA1_Channel1

#define ADCx_DMA_TX_RX_IRQHandler       DMA1_Channel1_IRQHandler    ///< ADC DMA中断服务函数
#define ADCx_IRQHandler                 ADC1_COMP_IRQHandler        ///< ADC中断服务函数

#define ADCx_DMA_TX_RX_IRQn             DMA1_Channel1_IRQn
#define ADCx_IRQn                       ADC1_COMP_IRQn


/**@brief       ADC初始化
* @return       函数执行结果
* - None
* @note         ADC通道通过ADC_CHANNEL_0_ENABLE, ADC_CHANNEL_1_ENABLE, ..., 
* ADC_CHANNEL_9_ENABLE, ADC_CHANNEL_TEMPSENSOR_ENABLE宏定义打开
*/
void BSP_ADC_Init(void);


/**@brief       ADC开始DMA传输
* @param[in]    pData : DMA接收缓存指针;
* @param[in]    Length : 缓存大小
* @return       函数执行结果
* - OP_SUCCESS(成功)
* - OP_FAILED(失败)
*/
uint8_t BSP_ADC_Start_DMA(uint32_t *pData, uint32_t Length);

/**@brief       ADC转换开始
* @return       函数执行结果
* - None
*/
void BSP_ADC_Conver_Start(void);

/**@brief       ADC转换停止
* @return       函数执行结果
* - None
*/
void BSP_ADC_Conver_Stop(void);


#ifdef __cplusplus
}
#endif
#endif /*__ adc_H */
