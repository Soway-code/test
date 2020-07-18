/**@file        usart_bsp.h
* @details      usart_bsp.c的头文件,声明了串口驱动的API函数
* @author       杨春林
* @date         2020-04-28
* @version      V1.0.0
* @copyright    2020-2030,深圳市信为科技发展有限公司
**********************************************************************************
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/04/28  <td>1.0.0    <td>杨春林    <td>创建初始版本
* </table>
*
**********************************************************************************
*/

#ifndef __usart_H
#define __usart_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "VariaType.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */
/* User can use this section to tailor USARTx/UARTx instance used and associated
   resources */
/* Definition for USARTx clock resources */
#define USARTx                           USART1
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE()
#define DMAx_CLK_ENABLE()                __HAL_RCC_DMA1_CLK_ENABLE()
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()

#define USARTx_CLK_DISABLE()             __HAL_RCC_USART1_CLK_DISABLE()

/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_9
#define USARTx_TX_GPIO_PORT              GPIOA
#define USARTx_TX_AF                     GPIO_AF1_USART1
#define USARTx_RX_PIN                    GPIO_PIN_10
#define USARTx_RX_GPIO_PORT              GPIOA
#define USARTx_RX_AF                     GPIO_AF1_USART1
#define UART_IT_RTO                         0x0B3AU                  /*!< UART receiver timeout interruption             */
#define UART_CLEAR_RTOF                      USART_ICR_RTOCF           /*!< UART receiver timeout clear flag  */
/* Definition for USARTx's DMA */
#define USARTx_TX_DMA_STREAM              DMA1_Channel2
#define USARTx_RX_DMA_STREAM              DMA1_Channel3

/* Definition for USARTx's NVIC */
#define USARTx_DMA_TX_RX_IRQn             DMA1_Channel2_3_IRQn
#define USARTx_DMA_TX_RX_IRQHandler       DMA1_Channel2_3_IRQHandler

/* Definition for USARTx's NVIC */
#define USARTx_IRQn                      USART1_IRQn
#define USARTx_IRQHandler                USART1_IRQHandler


#ifndef OP_SUCCESS
#define OP_SUCCESS
#endif

#ifndef OP_FAILED
#define OP_FAILED
#endif


/**@brief       串口初始化
* @param[in]    baudrate : 波特率; 
* @param[in]    parity : 奇偶校验;
* @return       函数执行结果
* - None
*/
void BSP_USART_UART_Init(uint32_t baudrate, uint32_t parity);

/**@brief       串口反初始化
* @return       函数执行结果
* - None
*/
void BSP_USART_UART_DeInit(void);
    
/**@brief       串口DMA发送
* @param[in]    pData : DMA发送缓存指针;
* @param[in]    Size : 缓存大小
* @return       函数执行结果
* - OP_SUCCESS(成功)
* - OP_FAILED(失败)
*/
uint8_t BSP_UART_Transmit_DMA(uint8_t *pData, uint16_t Size);

/**@brief       串口DMA接收
* @param[in]    pData : DMA接收缓存指针;
* @param[in]    Size : 缓存大小
* @return       函数执行结果
* - OP_SUCCESS(成功)
* - OP_FAILED(失败)
*/
uint8_t BSP_UART_Receive_DMA(uint8_t *pData, uint16_t Size);

/**@brief       串口配置接收超时中断
* @param[in]    TimeoutValue : 超时时间，单位波特率位数;
* @return       函数执行结果
* - None
* @note         示例：1个起始位，8位数据，1个停止位的串口配置，
* 则10位为1个数据字节，若设置4个数据字节的超时时间，
* TimeoutValue = 40
*/
void BSP_UART_ReceiverTimeout_Config(uint32_t TimeoutValue);

/**@brief       串口配置字符匹配中断
* @param[in]    Value : 匹配的字符;
* @return       函数执行结果
* - None
*/
void BSP_UART_CharMatch_Config(uint8_t Value);

/**@brief       串口接收超时中断回调函数
* @param[in]    huart : 串口处理对象;
* @return       函数执行结果
* - None
* @note         该函数可在其他文件中重定义，且是自定义的一个函数，在HAL库中不存在的
*/
void HAL_UART_RxTimoCallback(UART_HandleTypeDef *huart);

/**@brief       串口字符匹配中断回调函数
* @param[in]    huart : 串口处理对象;
* @return       函数执行结果
* - None
* @note         该函数可在其他文件中重定义，且是自定义的一个函数，在HAL库中不存在的
*/
void HAL_UART_CMatchCallback(UART_HandleTypeDef *huart);

#ifdef __cplusplus
}
#endif
#endif 
