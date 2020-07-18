/**@file        usart_bsp.c
* @brief        串口驱动
* @details      通用串口初始化, 为串口数据收发提供API函数
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

#include "usart_bsp.h"
#include "drv_common.h"
#include "stm32g0xx_hal_uart.h"
/* 使用RT-Thread操作系统,USING_RT_THREAD_OS在main.h中定义 */
#ifdef USING_RT_THREAD_OS
#include "board.h"
#endif


/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

static UART_HandleTypeDef huart;        ///< 串口处理对象
static DMA_HandleTypeDef hdma_usart_rx; ///< 串口接收DMA处理对象
static DMA_HandleTypeDef hdma_usart_tx; ///< 串口发送DMA处理对象


/**@brief       串口初始化
* @param[in]    baudrate : 波特率; 
* @param[in]    parity : 奇偶校验;
* @return       函数执行结果
* - None
*/
void BSP_USART_UART_Init(uint32_t baudrate, uint32_t parity)
{
    huart.Instance = USARTx;
    if(parity == UART_PARITY_NONE)
    {
        huart.Init.WordLength = UART_WORDLENGTH_8B;
    }
    else if(parity == UART_PARITY_ODD 
        || parity == UART_PARITY_EVEN)
    {
        huart.Init.WordLength = UART_WORDLENGTH_9B;
    }
    huart.Init.StopBits = UART_STOPBITS_1;
    huart.Init.Mode = UART_MODE_TX_RX;
    huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart.Init.OverSampling = UART_OVERSAMPLING_16;
    huart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT; 
	huart.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	
    huart.Init.BaudRate = baudrate;
    huart.Init.Parity = parity;

    if (HAL_UART_Init(&huart) != HAL_OK)
    {
        Error_Handler();
    }
    
	if (HAL_UARTEx_SetTxFifoThreshold(&huart, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&huart) != HAL_OK)
	{
		Error_Handler();
	}
}

/**@brief       串口反初始化
* @return       函数执行结果
* - None
*/
void BSP_USART_UART_DeInit(void)
{
    if (HAL_UART_DeInit(&huart) != HAL_OK)
    {
        Error_Handler();
    }
}

/**@brief       串口时钟、底层管脚初始化(由HAL_UART_Init回调调用)
* @param[in]    uartHandle : 串口处理对象
* @return       函数执行结果
* - None
* @note         串口的时钟、管脚、中断服务函数类型等在usart_bsp.h文件里面定义
*/
void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    USARTx_CLK_ENABLE();
    DMAx_CLK_ENABLE();
  
    USARTx_TX_GPIO_CLK_ENABLE();
    USARTx_RX_GPIO_CLK_ENABLE();

    GPIO_InitStruct.Pin = USARTx_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = USARTx_TX_AF;
    HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);
      
    GPIO_InitStruct.Pin = USARTx_RX_PIN;
    GPIO_InitStruct.Alternate = USARTx_RX_AF;
    HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);

    /* USART1 DMA Init */
    /* USART1_RX Init */
    hdma_usart_rx.Instance = USARTx_RX_DMA_STREAM;
	hdma_usart_rx.Init.Request = DMA_REQUEST_USART1_RX;
    hdma_usart_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart_rx.Init.Priority = DMA_PRIORITY_HIGH;
    if (HAL_DMA_Init(&hdma_usart_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart_rx);

    /* USART1_TX Init */
    hdma_usart_tx.Instance = USARTx_TX_DMA_STREAM;
	hdma_usart_tx.Init.Request = DMA_REQUEST_USART1_TX;
    hdma_usart_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart_tx.Init.Mode = DMA_NORMAL;
    hdma_usart_tx.Init.Priority = DMA_PRIORITY_HIGH;
    if (HAL_DMA_Init(&hdma_usart_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart_tx);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USARTx_DMA_TX_RX_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USARTx_DMA_TX_RX_IRQn);
    
    HAL_NVIC_SetPriority(USARTx_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USARTx_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
}

/**@brief       串口时钟、底层管脚反初始化(由HAL_UART_DeInit回调调用)
* @param[in]    uartHandle : 串口处理对象
* @return       函数执行结果
* - None
* @note         串口的时钟、管脚、中断服务函数类型等在usart_bsp.h文件里面定义
*/
void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

    USARTx_CLK_DISABLE();
  
    HAL_GPIO_DeInit(USARTx_TX_GPIO_PORT, USARTx_TX_PIN);
    HAL_GPIO_DeInit(USARTx_RX_GPIO_PORT, USARTx_RX_PIN);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USARTx_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */

} 

/**@brief       串口DMA发送
* @param[in]    pData : DMA发送缓存指针;
* @param[in]    Size : 缓存大小
* @return       函数执行结果
* - OP_SUCCESS(成功)
* - OP_FAILED(失败)
*/
uint8_t BSP_UART_Transmit_DMA(uint8_t *pData, uint16_t Size)
{
    if(HAL_UART_Transmit_DMA(&huart, pData, Size) != HAL_OK)
    {
        return OP_FAILED;
    }
    return OP_SUCCESS;
}

/**@brief       串口DMA接收
* @param[in]    pData : DMA接收缓存指针;
* @param[in]    Size : 缓存大小
* @return       函数执行结果
* - OP_SUCCESS(成功)
* - OP_FAILED(失败)
*/
uint8_t BSP_UART_Receive_DMA(uint8_t *pData, uint16_t Size)
{
    if(HAL_UART_Receive_DMA(&huart, pData, Size) != HAL_OK)
    {
        return OP_FAILED;
    }
    return OP_SUCCESS;
}

void HAL_UART_ReceiverTimeout_Config(UART_HandleTypeDef *huart, uint32_t TimeoutValue)
{
  assert_param(IS_UART_RECEIVER_TIMEOUT_VALUE(TimeoutValue));
  MODIFY_REG(huart->Instance->RTOR, USART_RTOR_RTO, TimeoutValue);
}
HAL_StatusTypeDef HAL_UART_DisableReceiverTimeout(UART_HandleTypeDef *huart)
{
  if (huart->gState == HAL_UART_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(huart);

    huart->gState = HAL_UART_STATE_BUSY;

    /* Clear the USART RTOEN bit */
    CLEAR_BIT(huart->Instance->CR2, USART_CR2_RTOEN);

    huart->gState = HAL_UART_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(huart);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}

HAL_StatusTypeDef HAL_UART_EnableReceiverTimeout(UART_HandleTypeDef *huart)
{
  if (huart->gState == HAL_UART_STATE_READY)
  {
    /* Process Locked */
    __HAL_LOCK(huart);

    huart->gState = HAL_UART_STATE_BUSY;

    /* Set the USART RTOEN bit */
    SET_BIT(huart->Instance->CR2, USART_CR2_RTOEN);

    huart->gState = HAL_UART_STATE_READY;

    /* Process Unlocked */
    __HAL_UNLOCK(huart);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}


/**@brief       串口配置接收超时中断
* @param[in]    TimeoutValue : 超时时间，单位波特率位数;
* @return       函数执行结果
* - None
* @note         示例：1个起始位，8位数据，1个停止位的串口配置，
* 则10位为1个数据字节，若设置4个数据字节的超时时间，
* TimeoutValue = 40
*/
void BSP_UART_ReceiverTimeout_Config(uint32_t TimeoutValue)
{
    HAL_UART_DisableReceiverTimeout(&huart);
    __HAL_UART_DISABLE_IT(&huart, UART_IT_RTO);
    
    HAL_UART_ReceiverTimeout_Config(&huart, TimeoutValue);
    
    HAL_UART_EnableReceiverTimeout(&huart);
    __HAL_UART_ENABLE_IT(&huart, UART_IT_RTO);
}

/**@brief       串口配置字符匹配中断
* @param[in]    Value : 匹配的字符;
* @return       函数执行结果
* - None
*/
void BSP_UART_CharMatch_Config(uint8_t Value)
{
    uint32_t usart_cr1;
    
    assert_param(IS_UART_ADDRESSLENGTH_DETECT(WakeUpSelection.AddressLength));
    
    usart_cr1 = READ_BIT(huart.Instance->CR1, USART_CR1_RE | USART_CR1_UE);
    CLEAR_BIT(huart.Instance->CR1, usart_cr1);
    __HAL_UART_DISABLE_IT(&huart, UART_IT_CM);
  
    MODIFY_REG(huart.Instance->CR2, USART_CR2_ADDM7, UART_ADDRESS_DETECT_7B);
    MODIFY_REG(huart.Instance->CR2, USART_CR2_ADD, ((uint32_t)Value << UART_CR2_ADDRESS_LSB_POS));
    
    __HAL_UART_ENABLE_IT(&huart, UART_IT_CM);
    SET_BIT(huart.Instance->CR1, usart_cr1);
}

/**@brief       串口接收超时中断回调函数
* @param[in]    huart : 串口处理对象;
* @return       函数执行结果
* - None
* @note         该函数是个弱定义函数，可在其他文件中重定义，且是自定义的一个函数，在HAL库中不存在的
*/
__weak void HAL_UART_RxTimoCallback(UART_HandleTypeDef *huart)
{
    UNUSED(huart);
}

/**@brief       串口字符匹配中断回调函数
* @param[in]    huart : 串口处理对象;
* @return       函数执行结果
* - None
* @note         该函数是个弱定义函数，可在其他文件中重定义，且是自定义的一个函数，在HAL库中不存在的
*/
__weak void HAL_UART_CMatchCallback(UART_HandleTypeDef *huart)
{
    UNUSED(huart);
}

/**@brief       串口DMA中断服务函数
* @return       函数执行结果
* - None
* @note         USARTx_DMA_TX_RX_IRQHandler在usart_bsp.h中定义
*/
void USARTx_DMA_TX_RX_IRQHandler(void)
{
/* 使用RT-Thread操作系统,USING_RT_THREAD_OS在main.h中定义 */
#ifdef USING_RT_THREAD_OS
    rt_interrupt_enter();
#endif
    
    HAL_DMA_IRQHandler(&hdma_usart_tx);
    HAL_DMA_IRQHandler(&hdma_usart_rx);

/* 使用RT-Thread操作系统,USING_RT_THREAD_OS在main.h中定义 */
#ifdef USING_RT_THREAD_OS
    rt_interrupt_leave();
#endif
}


/**@brief       串口中断服务函数
* @return       函数执行结果
* - None
* @note         USARTx_IRQHandler在usart_bsp.h中定义
*/
void USARTx_IRQHandler(void)
{
/* 使用RT-Thread操作系统,USING_RT_THREAD_OS在main.h中定义 */
#ifdef USING_RT_THREAD_OS
    rt_interrupt_enter();
#endif
    
    if(__HAL_UART_GET_IT(&huart, UART_IT_RTO))
    {
        __HAL_UART_CLEAR_FLAG(&huart, UART_CLEAR_RTOF);    
        HAL_UART_RxTimoCallback(&huart);
    }
    
    if(__HAL_UART_GET_IT(&huart, UART_IT_CM))
    {
        __HAL_UART_CLEAR_FLAG(&huart, UART_CLEAR_CMF);    
        HAL_UART_CMatchCallback(&huart);
    }
    
    if(__HAL_UART_GET_IT(&huart, UART_IT_TC) && __HAL_UART_GET_IT_SOURCE(&huart, UART_IT_TC))
    {
        __HAL_UART_DISABLE_IT(&huart, UART_IT_TC);
        huart.gState = HAL_UART_STATE_READY;
        HAL_UART_TxCpltCallback(&huart);
    }
    
    if(__HAL_UART_GET_IT(&huart, UART_IT_PE))
    {
        __HAL_UART_CLEAR_FLAG(&huart, UART_CLEAR_PEF);    
    }
    
    if(__HAL_UART_GET_IT(&huart, UART_IT_FE))
    {
        __HAL_UART_CLEAR_FLAG(&huart, UART_CLEAR_FEF);    
    }
    
    if(__HAL_UART_GET_IT(&huart, UART_IT_NE))
    {
        __HAL_UART_CLEAR_FLAG(&huart, UART_CLEAR_NEF);    
    }
    
    if(__HAL_UART_GET_IT(&huart, UART_IT_ORE))
    {
        __HAL_UART_CLEAR_FLAG(&huart, UART_CLEAR_OREF);    
    }
    
	
/* 使用RT-Thread操作系统,USING_RT_THREAD_OS在main.h中定义 */
#ifdef USING_RT_THREAD_OS
    rt_interrupt_leave();
#endif
}

