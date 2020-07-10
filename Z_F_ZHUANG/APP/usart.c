/**
* @file         usart.c
* @brief        串口驱动
* @details      串口初始化、串口数据收发、中断处理
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

/* Includes ------------------------------------------------------------------*/
#include "usart.h"
#include "string.h"
#include "stdio.h"
#include "stdarg.h"
#include "type.h"
#include "modbus_ascii.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

uint8_t u8SendNum = 0;                  //当前发送数据个数
uint8_t u8SendIndex = 0;                //发送数据缓冲前缀
uint8_t UART1_RXBuffLen = 0;            //接收数据缓存长度


BitAction UartRecvFrameOK = Bit_RESET;  //接收数据完成标志
uint8_t UART1_RXBuff[USART_MAX_DATALEN];//接收数据缓存
uint8_t UART1_TXBuff[USART_MAX_DATALEN];//发送数据缓存


UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;
DMA_HandleTypeDef hdma_usart1_tx;


uint8_t rx_flag = 0;
uint16_t rx_len;

extern UserTypeDef UserPara;

/**
* @brief      串口初始化
* @retval     None
*/
void MX_USART1_UART_Init(void)
{

	huart1.Instance = USART1;
//	huart1.Init.BaudRate = 115200;

	switch(UserPara.Baudrate)
	{   
	case 01:  
		huart1.Init.BaudRate = 2400;         //配置波特率（波特率2400）
		break;
	case 02:
		huart1.Init.BaudRate = 4800;    
		break;
	case 03:
		huart1.Init.BaudRate = 9600;   
		break;
	case 04:  
		huart1.Init.BaudRate = 19200;   
		break;
	case 05:
		huart1.Init.BaudRate = 38400;
		break;
	case 06:
		huart1.Init.BaudRate = 57600;    
		break;
	case 07:
		huart1.Init.BaudRate = 115200;     
		break;
	default://默认9600
		huart1.Init.BaudRate = 9600;      //配置波特率（波特率9600）
		break;
	}
		   
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
//	huart1.Init.Parity = UART_PARITY_NONE;
	switch(UserPara.Parity)
	{
		case 03:
			huart1.Init.Parity = UART_PARITY_NONE;
			break;
		case 02:
			huart1.Init.Parity = UART_PARITY_EVEN;               //配置数据位，校验位，串口模式
			break;
		case 01:
			huart1.Init.Parity = UART_PARITY_ODD;               //配置数据位，校验位，串口模式
			break;
		default:
			huart1.Init.Parity = UART_PARITY_NONE;               //配置数据位，校验位，串口模式
			break;
	}
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
	{
		Error_Handler();
	}

	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);     // 开启空闲接收中断 
	//DMA接收函数，此句一定要加，不加接收不到第一次传进来的实数据，是空的，且此时接收到的数据长度为缓存器的数据长度
	HAL_UART_Receive_DMA(&huart1, UART1_RXBuff, USART_MAX_DATALEN);
}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 DMA Init */
    /* USART1_RX Init */
    hdma_usart1_rx.Instance = DMA1_Channel2;
    hdma_usart1_rx.Init.Request = DMA_REQUEST_USART1_RX;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_NORMAL;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart1_rx);

    /* USART1_TX Init */
    hdma_usart1_tx.Instance = DMA1_Channel3;
    hdma_usart1_tx.Init.Request = DMA_REQUEST_USART1_TX;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,hdma_usart1_tx);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
} 

/**
* @brief        串口DMA发送
* @param[in]    pData : DMA发送缓存指针;
* @param[in]    Size : 缓存大小
* @return       函数执行结果
* - success(成功)
* - fail(失败)
*/
uint8_t UART_DMA_Send(uint8_t *pData, uint16_t Size)
{
	if(HAL_UART_Transmit_DMA(&huart1, pData, Size) != HAL_OK)
	{
		return fail;
	}
	
	return success;
}

/**
* @brief       串口DMA接收
* @param[in]    pData : DMA接收缓存指针;
* @param[in]    Size : 缓存大小
* @return       函数执行结果
* - success(成功)
* - fail(失败)
*/
uint8_t UART_DMA_Recv(uint8_t *pData, uint16_t Size)
{
	if(HAL_UART_Receive_DMA(&huart1, pData, Size) != HAL_OK)
	{
		return fail;
	}
	
	return success;
}

/**
* @brief       串口中断服务函数
* @retval      None     
* - None
*/
void USART1_IRQHandler(void)
{
	uint8_t RecvByteData;
	
	HAL_UART_IRQHandler(&huart1);
	
	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE))
	{

		__HAL_UART_CLEAR_IDLEFLAG(&huart1);
		HAL_UART_DMAStop(&huart1);
		rx_len = USART_MAX_DATALEN - __HAL_DMA_GET_COUNTER(&hdma_usart1_rx);
//		rx_flag = 1;
	
		RecvByteData = USART1->RDR;
		
		UART_DMA_Send(UART1_RXBuff, rx_len);
		
		MODBUS_ASCII_HandlRevData(RecvByteData);
		
		HAL_UART_Receive_DMA(&huart1, UART1_RXBuff, USART_MAX_DATALEN);
	}

	if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_ORE))
	{
		__HAL_UART_CLEAR_FLAG(&huart1, UART_CLEAR_OREF);
	}
}

//void USART1_IRQHandler(void)
//{
//    uint8_t RecvByteData;
//	
//    if(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE))
//    {
//        __HAL_UART_CLEAR_IDLEFLAG(&huart1);
//        HAL_UART_DMAStop(&huart1);
//		
//        RecvByteData = USART1->RDR;
//		
////        MODBUS_ASCII_HandlRevData(RecvByteData); //接收ASCII数据    
//		
//		UART_DMA_Send(UART1_RXBuff, rx_len);
//		HAL_UART_Receive_DMA(&huart1, UART1_RXBuff, USART_MAX_DATALEN);
//    }
//	
//}
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
