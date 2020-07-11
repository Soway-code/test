#include "main.h"
#include "stdio.h"
#include "string.h"
#include "usart.h"

UART_HandleTypeDef hlpuart1;

uint8_t RecByteErr = 0;															    // ���ڽ����ֽڴ���(��żУ�������ֽ�֡����)
uint8_t RecPos = 0;															 	      // ���ڽ�����������
uint8_t RcvByteFlag = 0;														    // ���ڽ������ݱ�־
uint8_t RcvPacketLen = 0;														    // ���ڽ������ݰ�����
uint8_t RcvPacketFlag = 0;													    // ���ڽ������ݰ���־
uint8_t RecvByteData = 0;														    // ���ڽ��յ������ݻ���
uint8_t RecBuf[DataPacketLen] = {0};								    // ���ڽ������ݻ���

extern TIM_HandleTypeDef htim3;
extern uint16_t ParityErrorFlag;												// ������żУ������־
extern uint16_t Out_Put_Mode;
extern FlashPara_Typedef FlashPara;										// Flash��������ṹ��
extern uint8_t master_request_data[DataPacketLen];
extern uint32_t BaudRateTable[8];	

#if 1
//#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((LPUART1->ISR&0X40)==0);//ѭ������,ֱ���������   
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
		hlpuart1.Init.BaudRate = BaudRateTable[FlashPara.Customer.Baud];			// ����������
	
	hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;								// Ĭ��У�����ݳ���Ϊ8λ
	hlpuart1.Init.Parity = UART_PARITY_NONE;										// Ĭ��Ϊ��У��
	switch(FlashPara.Customer.Parity)
	{
		case PARITY_NONE:														// ��У�� �������ݳ���Ϊ8λ
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
  	HAL_NVIC_SetPriority(USART3_4_LPUART1_IRQn, 1, 0);									// ���ȼ�
    HAL_NVIC_EnableIRQ(USART3_4_LPUART1_IRQn);											// ʹ�����ж�
     
	__HAL_UART_ENABLE_IT(&hlpuart1, UART_IT_RXNE);								// ʹ�ܽ����ж�
	__HAL_UART_ENABLE(&hlpuart1);
  /* USER CODE END LPUART1_Init 2 */

}

void USART1_RecMod_RTU(void)
{ 
	TIM3->CNT = 0;
	if(RecPos == 0)
    	HAL_TIM_Base_Start(&htim3);
    
    RecBuf[RecPos++] = RecvByteData;											// ��������
    
    if(RecPos == DataPacketLen)													// �������泤�Ⱦ���ͷ��ʼ����
        RecPos = 0;
    
    RcvByteFlag = 1;															// ���ڽ��ձ�־
}

void USART1_RecMod_ASCII(void)
{
    if(RecPos == 0)																// ��һ������
    {
        if(RecvByteData == 0x3A && RecByteErr == 0)								// ���յ�һ��������֡ͷ(0x3A)���ֽ�֡�޴���Ϳ�ʼ����
		{
      RecBuf[RecPos++] = RecvByteData;
			ParityErrorFlag = 0;												// ���֡������ ������һ֡������Ϣ֮ǰ�����Ĵ���Ӧ�������
		}
    }
    
    else
    {
        RecBuf[RecPos++] =  RecvByteData;
        
        if(RecBuf[RecPos - 2] == 0x0D && RecBuf[RecPos - 1] == 0x0A)			// ֡β��־
        {
            RcvPacketLen = RecPos;                              				// �������ݳ���
            RcvPacketFlag = 1;                                  				// ����֡��־��1
            RecPos = 0;                                         				// ����������������
            MDB_Get_master_request_data(master_request_data, RecBuf, RcvPacketLen);	// �Ӵ������ݻ�������ȡ������ѯ��Ϣ֡����
        }
		
		if(RecPos == DataPacketLen)												// �������泤�Ⱦ���ͷ��ʼ����
        	RecPos = 0;
    }
}

// ��ȡ������ѯ֡����
void MDB_Get_master_request_data(uint8_t *destination, uint8_t *source, uint8_t len)
{
	uint8_t i = 0;
    
    for(i = 0; i < len; i++)
        destination[i] = source[i];
}

void USART3_4_LPUART1_IRQHandler(void)													// ���ڽ����жϺ���
{		
    if(__HAL_UART_GET_IT(&hlpuart1, UART_IT_RXNE) != RESET)						// �ж��Ƿ��ǽ����ж� 
    {
			if(__HAL_UART_GET_FLAG(&hlpuart1, UART_FLAG_PE) != RESET || 				// ��żУ��λ�����־
				 __HAL_UART_GET_FLAG(&hlpuart1, UART_FLAG_FE) != RESET)		   			// ֡�����־			
			{
				__HAL_UART_CLEAR_FLAG(&hlpuart1, UART_CLEAR_PEF | UART_CLEAR_FEF);	// ����żУ������־��֡�����־
				RecByteErr = 1;														// ������ ���ڽ����ֽڴ���(��żУ�������ֽ�֡����)
				ParityErrorFlag = 1;												// ֡������
			}

			RecvByteData = LPUART1->RDR;												// ��ȡ��������Ӳ�����Զ����жϱ�־

			if(Out_Put_Mode == OUTPUTMOD_RTU)										// RTUģʽ �˴�����CustomerPara.OutPutMod��������Ϊ�ñ����ڱ���λ��дʱ��ʵʱ���µ� ����ɱ������ģʽ�ı�
			USART1_RecMod_RTU();												// ��ʵ��Ҫ��Ӧ���ǵ�ǰ֡Ӧ������� �´���Ч Out_Put_Mode���������𵽴�����
					
			else if(Out_Put_Mode == OUTPUTMOD_ASCII)									// ASCIIģʽ
			USART1_RecMod_ASCII();

			RecByteErr = 0;															// ��������� ���ڽ����ֽڴ���(��żУ�������ֽ�֡����)
    }
	
		if(__HAL_UART_GET_FLAG(&hlpuart1, UART_FLAG_ORE) != RESET) 					// �����д��ж� ��дFlash�����л�ر����ж� ���ڴ�ʱ��λ���������ݹ���������ɴ���������� ������������־�ᵼ�´��ڽ�������
		{ 
			__HAL_UART_CLEAR_FLAG(&hlpuart1, UART_CLEAR_OREF);						// �������ж� 
		}
}



