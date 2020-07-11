#include "main.h"
#include "stdio.h"
#include "string.h"
#include "usart.h"

UART_HandleTypeDef hlpuart1;

uint8_t RecByteErr = 0;															    // 串口接收字节错误(奇偶校验错误或字节帧错误)
uint8_t RecPos = 0;															 	      // 串口接收数据索引
uint8_t RcvByteFlag = 0;														    // 串口接收数据标志
uint8_t RcvPacketLen = 0;														    // 串口接收数据包长度
uint8_t RcvPacketFlag = 0;													    // 串口接收数据包标志
uint8_t RecvByteData = 0;														    // 串口接收单个数据缓存
uint8_t RecBuf[DataPacketLen] = {0};								    // 串口接收数据缓存

extern TIM_HandleTypeDef htim3;
extern uint16_t ParityErrorFlag;												// 串口奇偶校验错误标志
extern uint16_t Out_Put_Mode;
extern FlashPara_Typedef FlashPara;										// Flash保存参数结构体
extern uint8_t master_request_data[DataPacketLen];
extern uint32_t BaudRateTable[8];	

#if 1
//#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((LPUART1->ISR&0X40)==0);//循环发送,直到发送完毕   
	LPUART1->TDR=(uint8_t)ch;      
	return ch;
}
#endif 

/**
  * @brief LPUART1 Initialization Function
  * @param None
  * @retval None
  */
void MX_LPUART1_UART_Init(void)
{

  /* USER CODE BEGIN LPUART1_Init 0 */

  /* USER CODE END LPUART1_Init 0 */

  /* USER CODE BEGIN LPUART1_Init 1 */

  /* USER CODE END LPUART1_Init 1 */
  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 9600;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  hlpuart1.FifoMode = UART_FIFOMODE_DISABLE;

	
  hlpuart1.Init.BaudRate = 9600;
	if(FlashPara.Customer.Baud >= 1 && FlashPara.Customer.Baud <= 7)
		hlpuart1.Init.BaudRate = BaudRateTable[FlashPara.Customer.Baud];			// 波特率配置
	
	hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;								// 默认校验数据长度为8位
	hlpuart1.Init.Parity = UART_PARITY_NONE;										// 默认为无校验
	switch(FlashPara.Customer.Parity)
	{
		case PARITY_NONE:														// 奇校验 配置数据长度为8位
			hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
			hlpuart1.Init.Parity = UART_PARITY_NONE;
			break;
			
		case PARITY_ODD:
			hlpuart1.Init.WordLength = UART_WORDLENGTH_9B;
			hlpuart1.Init.Parity = UART_PARITY_ODD;
			break;
		
		default:
			hlpuart1.Init.WordLength = UART_WORDLENGTH_9B;						
			hlpuart1.Init.Parity = UART_PARITY_EVEN;							
			break;
	}
	
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&hlpuart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&hlpuart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPUART1_Init 2 */
  	HAL_NVIC_SetPriority(USART3_4_LPUART1_IRQn, 1, 0);									// 优先级
    HAL_NVIC_EnableIRQ(USART3_4_LPUART1_IRQn);											// 使能总中断
     
	__HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_RXNE);								// 使能接收中断
	__HAL_UART_ENABLE(&hlpuart1);
  /* USER CODE END LPUART1_Init 2 */

}

void USART1_RecMod_RTU(void)
{ 
	TIM3->CNT = 0;
	if(RecPos == 0)
    	HAL_TIM_Base_Start(&htim3);
    
    RecBuf[RecPos++] = RecvByteData;											// 接收数据
    
    if(RecPos == DataPacketLen)													// 超出缓存长度就重头开始缓存
        RecPos = 0;
    
    RcvByteFlag = 1;															// 串口接收标志
}

void USART1_RecMod_ASCII(void)
{
    if(RecPos == 0)																// 第一个数据
    {
        if(RecvByteData == 0x3A && RecByteErr == 0)								// 接收第一个数据是帧头(0x3A)且字节帧无错误就开始接收
		{
      RecBuf[RecPos++] = RecvByteData;
			ParityErrorFlag = 0;												// 清除帧错误标记 发生在一帧正常消息之前发生的错误应当被清除
		}
    }
    
    else
    {
        RecBuf[RecPos++] =  RecvByteData;
        
        if(RecBuf[RecPos - 2] == 0x0D && RecBuf[RecPos - 1] == 0x0A)			// 帧尾标志
        {
            RcvPacketLen = RecPos;                              				// 接收数据长度
            RcvPacketFlag = 1;                                  				// 接收帧标志置1
            RecPos = 0;                                         				// 接收数据索引清零
            MDB_Get_master_request_data(master_request_data, RecBuf, RcvPacketLen);	// 从串口数据缓冲区获取主机查询消息帧数据
        }
		
		if(RecPos == DataPacketLen)												// 超出缓存长度就重头开始缓存
        	RecPos = 0;
    }
}

// 获取主机查询帧数据
void MDB_Get_master_request_data(uint8_t *destination, uint8_t *source, uint8_t len)
{
	uint8_t i = 0;
    
    for(i = 0; i < len; i++)
        destination[i] = source[i];
}

void USART3_4_LPUART1_IRQHandler(void)													// 串口接收中断函数
{		
    if(__HAL_UART_GET_IT(&hlpuart1, UART_IT_RXNE) != RESET)						// 判断是否是接收中断 
    {
			if(__HAL_UART_GET_FLAG(&hlpuart1, UART_FLAG_PE) != RESET || 				// 奇偶校验位错误标志
				 __HAL_UART_GET_FLAG(&hlpuart1, UART_FLAG_FE) != RESET)		   			// 帧错误标志			
			{
				__HAL_UART_CLEAR_FLAG(&hlpuart1, UART_CLEAR_PEF | UART_CLEAR_FEF);	// 清奇偶校验错误标志和帧错误标志
				RecByteErr = 1;														// 错误标记 串口接收字节错误(奇偶校验错误或字节帧错误)
				ParityErrorFlag = 1;												// 帧错误标记
			}

			RecvByteData = LPUART1->RDR;												// 读取串口数据硬件会自动清中断标志

			if(Out_Put_Mode == OUTPUTMOD_RTU)										// RTU模式 此处不用CustomerPara.OutPutMod变量是因为该变量在被上位机写时是实时更新的 会造成本次输出模式改变
			USART1_RecMod_RTU();												// 而实际要求应该是当前帧应答结束后 下次生效 Out_Put_Mode变量就是起到此作用
					
			else if(Out_Put_Mode == OUTPUTMOD_ASCII)									// ASCII模式
			USART1_RecMod_ASCII();

			RecByteErr = 0;															// 错误标记清除 串口接收字节错误(奇偶校验错误或字节帧错误)
    }
	
		if(__HAL_UART_GET_FLAG(&hlpuart1, UART_FLAG_ORE) != RESET) 					// 必须有此判断 擦写Flash过程中会关闭总中断 若在此时上位机发送数据过来，会造成串口溢出错误 不清溢出错误标志会导致串口接收死机
		{ 
			__HAL_UART_CLEAR_FLAG(&hlpuart1, UART_CLEAR_OREF);						// 清除溢出中断 
		}
}



