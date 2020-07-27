/**@file        usart_app.c
* @brief        串口应用
* @details      串口初始化、串口数据收发、中断处理
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

#include "usart_app.h"


static uint16_t RX_Len = 0;                     ///< 串口接收数据长度
static uint16_t Used_RX_Len = 0;                ///< 串口接收缓存被占用长度
static uint8_t RX_OverRun_Flag = 0;             ///< 串口接收缓存溢出标志
static uint8_t TX_Cplt_Flag = 1;                ///< 串口发送完成标志
static uint8_t RX_Updata_Flag = 0;              ///< 串口接收更新标志
static uint8_t USART_Receive_Buf[RECEIVE_SIZE]; ///< 串口接收缓存


#ifdef  USART_USING_485

/**@brief       初始化485芯片的发送接收控制管脚
* @return       函数执行结果
* - None
* @note         可通过宏定义USART_USING_485开启或关闭这个函数
*/
static void _485_RE_DE_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  _485_RE_DE_PIN_CLK_ENABLE();

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = _485_RE_DE_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(_485_RE_DE_GPIO_PORT, &GPIO_InitStruct);
  
  RX_ON;
}
#endif

/**@brief       根据波特率和奇偶校验参数初始化传感器串口
* @param[in]    baudrate_code : 波特率代码; 
* @param[in]    parity_code : 奇偶校验类型代码;
* @return       函数执行结果
* - None
*/
void Sensor_USART_Init(uint8_t baudrate_code, uint8_t parity_code)
{
    uint32_t baudrate;
    uint32_t parity;
#ifdef  USING_UART_TIMEOUT
    uint32_t timeout;
#endif
//    baudrate_code = 1;
    switch(baudrate_code)
    {
        case USART_BAUDRATE_4800_CODE : baudrate = 4800;
        break;
        
        case USART_BAUDRATE_9600_CODE : baudrate = 9600;
        break;
        
        case USART_BAUDRATE_19200_CODE : baudrate = 19200;
        break;
        
        case USART_BAUDRATE_38400_CODE : baudrate = 38400;
        break;
        
        case USART_BAUDRATE_57600_CODE : baudrate = 57600;
        break;
        
        case USART_BAUDRATE_115200_CODE : baudrate = 115200;
        break;
        
        default : baudrate = DEFAULT_UART_BAUD;
        break;
    }
    switch(parity_code)
    {
        case USART_PARITY_NONE_CODE : parity = UART_PARITY_NONE;
        break;
        
        case USART_PARITY_ODD_CODE : parity = UART_PARITY_ODD;
        break;
        
        case USART_PARITY_EVEN_CODE : parity = UART_PARITY_EVEN;
        break;
        
        default : parity = UART_PARITY_NONE;
        break;
    }
   
    BSP_USART_UART_Init(baudrate, parity);
#ifdef  USART_USING_485
    _485_RE_DE_GPIO_Init();
#endif
    
#ifdef  USING_UART_TIMEOUT
    if(parity_code == USART_PARITY_NONE_CODE)
    {
        timeout = DEFAULT_UART_TIMEOUT * 10;
    }
    else
    {
        timeout = DEFAULT_UART_TIMEOUT * 11;
    }
    timeout = (timeout % 10) ? ((timeout / 10) + 1) : timeout / 10;
    BSP_UART_ReceiverTimeout_Config(timeout);
#endif
    
#ifdef  USING_CHARMATCH
    BSP_UART_CharMatch_Config(DEFAULT_UART_MATCHCHAR);
#endif
    BSP_UART_Receive_DMA(USART_Receive_Buf, RECEIVE_SIZE);
}

/**@brief       获取实际接收的数据
* @param[out]   ReceData : 保存实际接收数据的缓存指针
* @return       函数执行结果
* - 获得的数据长度
* @note         由于使用了DMA循环模式,必须经过本函数才能保证接收数据的完整
*/
uint16_t Sensor_USART_GetReceData(uint8_t *ReceData)
{
    if(Used_RX_Len > RECEIVE_SIZE || RX_Len > RECEIVE_SIZE)
    {
        return 0;
    }
    
    if(Used_RX_Len > RX_Len)
    {
        memcpy( ReceData, USART_Receive_Buf 
                + Used_RX_Len
                - RX_Len, 
                RX_Len);
    }
    else
    {
        memcpy( ReceData, USART_Receive_Buf 
                + (RECEIVE_SIZE - RX_Len 
                + Used_RX_Len), 
                RX_Len - Used_RX_Len);
        memcpy( ReceData + RX_Len - Used_RX_Len, 
                USART_Receive_Buf, 
                Used_RX_Len);
    }
    return RX_Len;
}

/**@brief       获取传感器串口一次接收到的数据长度
* @return       函数执行结果
* - 一次接收到的数据长度
*/
uint16_t Sensor_USART_Get_RX_Len(void)
{
    return RX_Len;
}

/**@brief       获取传感器串口接收到数据后，串口接收缓存被占用长度
* @return       函数执行结果
* - 串口接收缓存被占用长度
*/
uint16_t Sensor_USART_Get_RXBuf_Used_Len(void)
{
    return Used_RX_Len;
}

/* 使用RT-Thread操作系统,USING_RT_THREAD_OS在main.h中定义 */
#ifdef USING_RT_THREAD_OS
#include <rtthread.h>

#define USART_SEM_NAME      "usart"

static struct rt_semaphore usart_sem;
static struct rt_semaphore usart_tx_lock;

/**
* 名称       : stm32_usart_sem_init()
* 创建日期   : 2020-05-18
* 作者       : 杨春林
* 功能       : 串口信号量初始化
* 输入参数   : 无
* 输出参数   : 无
* 返回结果   : int整型值
* 注意和说明 : 本函数使用RT-Thread的自动初始化组件INIT_DEVICE_EXPORT
               调用执行,系统复位后自动初始化
* 修改内容   :
*/
/**@brief       串口信号量初始化
* @return       int整型值
* - RT_ERROR(信号量初始化失败)
* - RT_EOK(信号量初始化成功)
*/
static int stm32_usart_sem_init(void)
{
    if( rt_sem_init(&usart_sem, USART_SEM_NAME, 0, RT_IPC_FLAG_FIFO) != RT_EOK
        || rt_sem_init(&usart_tx_lock, USART_TX_LOCK_NAME, 1, RT_IPC_FLAG_FIFO) != RT_EOK)
    {
        return RT_ERROR;
    }
    
    return RT_EOK;        
}
INIT_DEVICE_EXPORT(stm32_usart_sem_init);

#endif

/**@brief       获取接收更新标志
* @return       函数执行结果
* - 接收更新标志
*/
uint8_t Sensor_USART_Get_RX_Updata_Flag(void)
{
/* 使用RT-Thread操作系统,USING_RT_THREAD_OS在main.h中定义 */
#ifdef USING_RT_THREAD_OS
    rt_sem_take(&usart_sem, RT_WAITING_FOREVER);    //获取信号量
#endif
    return RX_Updata_Flag;
}

/**@brief       获取发送完成标志
* @return       函数执行结果
* - 发送完成标志
*/
uint8_t Sensor_USART_Get_TX_Cplt_Flag(void)
{  
    return TX_Cplt_Flag;
}

/**@brief       清除接收更新标志
* @return       函数执行结果
* - None
*/
void Sensor_USART_Clear_RX_Updata_Flag(void)
{
    RX_Updata_Flag = 0;
}

/**@brief       清除发送完成标志
* @return       函数执行结果
* - None
*/
void Sensor_USART_Clear_TX_Cplt_Flag(void)
{
    TX_Cplt_Flag = 0;
}

#ifdef  USING_UART_TIMEOUT
/**@brief       串口接收超时中断回调函数
* @param[out]   huart : 串口处理对象指针
* @return       函数执行结果
* - None
* @note         本函数自定义的一个函数，在HAL库中不存在的
*/
void HAL_UART_RxTimoCallback(UART_HandleTypeDef *huart)
{
    uint16_t rx_buf_used;
    
    rx_buf_used = huart->RxXferSize - __HAL_DMA_GET_COUNTER(huart->hdmarx);
    if(RX_OverRun_Flag)
    {
        RX_Len = rx_buf_used + huart->RxXferSize - Used_RX_Len;        
        RX_OverRun_Flag = 0;
    }
    else
    {
        RX_Len = rx_buf_used - Used_RX_Len;    
    }
    Used_RX_Len = rx_buf_used;
    
/* 使用RT-Thread操作系统,USING_RT_THREAD_OS在main.h中定义 */    
#ifdef USING_RT_THREAD_OS
    rt_sem_release(&usart_sem);     //释放信号量
#endif
    RX_Updata_Flag = 1;
}
#endif

#ifdef  USING_CHARMATCH
/**@brief       串口字符匹配中断回调函数
* @param[out]   huart : 串口处理对象指针
* @return       函数执行结果
* - None
* @note         本函数自定义的一个函数，在HAL库中不存在的
*/
void HAL_UART_CMatchCallback(UART_HandleTypeDef *huart)
{
    uint16_t rx_buf_used;
    
    rx_buf_used = huart->RxXferSize - __HAL_DMA_GET_COUNTER(huart->hdmarx);
    if(RX_OverRun_Flag)
    {
        RX_Len = rx_buf_used + huart->RxXferSize - Used_RX_Len;        
        RX_OverRun_Flag = 0;
    }
    else
    {
        RX_Len = rx_buf_used - Used_RX_Len;    
    }
    Used_RX_Len = rx_buf_used;
    
/* 使用RT-Thread操作系统,USING_RT_THREAD_OS在main.h中定义 */    
#ifdef USING_RT_THREAD_OS
    rt_sem_release(&usart_sem);     //释放信号量
#endif
    RX_Updata_Flag = 1;
}
#endif

/**@brief       串口接收完成中断回调函数,这里串口接收完成表示接收的数据满了或溢出了
* @param[out]   huart : 串口处理对象指针
* @return       函数执行结果
* - None
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    RX_OverRun_Flag = 1;
}  

/**@brief       串口发送完成中断回调函数
* @param[out]   huart : 串口处理对象指针
* @return       函数执行结果
* - None
*/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
#ifdef  USART_USING_485
    RX_ON;
#endif
/* 使用RT-Thread操作系统,USING_RT_THREAD_OS在main.h中定义 */    
#ifdef USING_RT_THREAD_OS
    rt_sem_release(&usart_tx_lock);     //释放信号量
#endif
    TX_Cplt_Flag = 1;
}
