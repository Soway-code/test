/**@file        usart_app.h
* @details      usart_app.c的头文件,定义了串口应用的宏定义,声明了串口应用的API函数
* @author       杨春林
* @date         2020-04-29
* @version      V1.0.0
* @copyright    2020-2030,深圳市信为科技发展有限公司
**********************************************************************************
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/04/29  <td>1.0.0    <td>杨春林    <td>创建初始版本
* </table>
*
**********************************************************************************
*/

#ifndef __USART_APP_H
#define __USART_APP_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "usart_bsp.h"
#include "string.h"

#define USART_BAUDRATE_4800_CODE        0   ///< 波特率代码,代表4800 bit/s
#define USART_BAUDRATE_9600_CODE        1   ///< 波特率代码,代表9600 bit/s
#define USART_BAUDRATE_19200_CODE       2   ///< 波特率代码,代表19200 bit/s
#define USART_BAUDRATE_38400_CODE       3   ///< 波特率代码,代表38400 bit/s
#define USART_BAUDRATE_57600_CODE       4   ///< 波特率代码,代表57600 bit/s
#define USART_BAUDRATE_115200_CODE      5   ///< 波特率代码,代表115200 bit/s

#define USART_PARITY_NONE_CODE          0   ///< 奇偶校验代码,代表无奇偶校验
#define USART_PARITY_ODD_CODE           1   ///< 奇偶校验代码,代表奇校验
#define USART_PARITY_EVEN_CODE          2   ///< 奇偶校验代码,代表偶校验

/** 默认波特率 9600 bit/s */
#define DEFAULT_UART_BAUD               9600    

/** 接收缓存的大小 */
#define RECEIVE_SIZE                    600

#define USING_UART_TIMEOUT
//#define USING_CHARMATCH

#ifdef  USING_UART_TIMEOUT
/** 默认超时时间，10表示1个字节时间 */
#define DEFAULT_UART_TIMEOUT            10
#endif

#ifdef  USING_CHARMATCH
/** 默认超时时间，10表示1个字节时间 */
#define DEFAULT_UART_MATCHCHAR          0x0A
#endif

/** 串口485收发控制管脚使能 */
#define USART_USING_485

#ifdef  USART_USING_485
#define _485_RE_DE_PIN_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define _485_RE_DE_PIN                  GPIO_PIN_8
#define _485_RE_DE_GPIO_PORT            GPIOA

#define TX_ON   HAL_GPIO_WritePin(_485_RE_DE_GPIO_PORT, _485_RE_DE_PIN, GPIO_PIN_SET)
#define RX_ON   HAL_GPIO_WritePin(_485_RE_DE_GPIO_PORT, _485_RE_DE_PIN, GPIO_PIN_RESET)
#endif

/* 使用RT-Thread操作系统,USING_RT_THREAD_OS在main.h中定义 */
#ifdef USING_RT_THREAD_OS

#define USART_TX_LOCK_NAME  "tx_lock"

#endif


/**@brief       根据波特率和奇偶校验参数初始化传感器串口
* @param[in]    baudrate_code : 波特率代码; 
* @param[in]    parity_code : 奇偶校验类型代码;
* @return       函数执行结果
* - None
*/
void Sensor_USART_Init(uint8_t baudrate_code, uint8_t parity_code);

/**@brief       获取实际接收的数据
* @param[out]   ReceData : 保存实际接收数据的缓存指针
* @return       函数执行结果
* - 获得的数据长度
* @note         由于使用了DMA循环模式,必须经过本函数才能保证接收数据的完整
*/
uint16_t Sensor_USART_GetReceData(uint8_t *ReceData);
    
/**@brief       获取传感器串口一次接收到的数据长度
* @return       函数执行结果
* - 一次接收到的数据长度
*/
uint16_t Sensor_USART_Get_RX_Len(void);

/**@brief       获取传感器串口接收到数据后，串口接收缓存被占用长度
* @return       函数执行结果
* - 串口接收缓存被占用长度
*/
uint16_t Sensor_USART_Get_RXBuf_Used_Len(void);

/**@brief       获取接收更新标志
* @return       函数执行结果
* - 接收更新标志
*/
uint8_t Sensor_USART_Get_RX_Updata_Flag(void);

/**@brief       获取发送完成标志
* @return       函数执行结果
* - 发送完成标志
*/
uint8_t Sensor_USART_Get_TX_Cplt_Flag(void);

/**@brief       清除接收更新标志
* @return       函数执行结果
* - None
*/
void Sensor_USART_Clear_RX_Updata_Flag(void);

/**@brief       清除发送完成标志
* @return       函数执行结果
* - None
*/
void Sensor_USART_Clear_TX_Cplt_Flag(void);

#ifdef __cplusplus
}
#endif
#endif
