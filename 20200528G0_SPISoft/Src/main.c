
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stdio.h"
#include "usart.h"


#define FACTOR_40    78    //��40g�ķֱ��ʦ�g
#define FACTOR_20	 39    //��20g
#define FACTOR_10    19.5f //��10g



extern uint16_t CycleTable[7];
extern uint16_t FreTable[8];
extern uint8_t RcvPacketLen;													// ���յ�������Ϣ֡����
extern uint8_t RcvPacketFlag;													// ���յ�������Ϣ֡��־
extern CustomerPara_Typedef Default_CustomerPara;								// �ͻ�����Ĭ�ϳ�ʼֵ

uint16_t ParityErrorFlag = 0;													// ������żУ������־
uint16_t Out_Put_Mode = 0;														// ���ģʽ����
uint16_t SamplingFlag = SamplingUnfinish;										// ������ɱ�־ ��������ɺ� ��TIM4�ж��л���Ϊ���
uint16_t SamplingCount = 0;														// ��������
FlashPara_Typedef	  FlashPara = {0};											// Flash��������ṹ��
uint8_t master_request_data[DataPacketLen] = {0};                               // ����������ѯ��Ϣ֡,���150��

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
uint8_t datax[3] = {0};
uint8_t datay[3] = {0};
uint8_t dataz[3] = {0};
//Remap address define
#define APPLICATION_ADDRESS     (uint32_t)0x8004000

//Define variable
    #if   (defined ( __CC_ARM ))
    __IO uint32_t VectorTable[48] __attribute__((at(0x20000000)));
    #elif (defined (__ICCARM__))
    #pragma location = 0x20000000
    __no_init __IO uint32_t VectorTable[48];
    #elif defined   (  __GNUC__  )
    __IO uint32_t VectorTable[48] __attribute__((section(".RAMVectorTable")));
    #elif defined ( __TASKING__ )
    __IO uint32_t VectorTable[48] __at(0x20000000);
    #endif

void RemapInterruptVerctor( void )
{
    for(uint32_t i = 0; i < 48; i++)
    {
        VectorTable[i] = *(__IO uint32_t*)(APPLICATION_ADDRESS + (i<<2));
    }
	
	//���� SYSCFG ʱ��
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	//��ӳ�� SRAM ��ַ�� 0x00000000
	__HAL_SYSCFG_REMAPMEMORY_SRAM();
}

int tempx,tempy,tempz;
int main(void)
{
    RemapInterruptVerctor();
	
	HAL_Init();
	SystemClock_Config();
	FlashParaInit();      // Flash������ʼ�� ��ȡ�򱣴�Flash���� ���������߽�Ĳ���ȡ���ݲ�������ֵ(Ĭ��ֵ)
	ParaInit();					  // ����Flash������ �ٳ�ʼ�����������
	MX_TIM1_Init();
    MX_TIM3_Init();
    MX_LPUART1_UART_Init();
	MX_GPIO_Init();
    RS485_RX_EN;
    
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
	SPI_Send_8Bit(0x58);
	SPI_Send_8Bit(0x83);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
	SPI_Send_8Bit(0x5A);
	SPI_Send_8Bit(0x00);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	while (1)
	{
	
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
//		SPI_Send_8Bit(0x11);
//		datax[0] = SPI_Receive_8Bit();
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
//		
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
//		SPI_Send_8Bit(0x13);
//		datax[1] = SPI_Receive_8Bit();
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
//		
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
//		SPI_Send_8Bit(0x15);
//		datax[2] = SPI_Receive_8Bit();
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
//		
//		
//		tempx=((((datax[0]<<16)|(datax[1]<<8)|(datax[2]&0xF0))<<8)>>12)*FACTOR_40;
//		
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
//	    SPI_Send_8Bit(0x17);
//		datay[0] = SPI_Receive_8Bit();
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
//		
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
//	    SPI_Send_8Bit(0x19);
//		datay[1] = SPI_Receive_8Bit();
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
//		
//		
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
//		SPI_Send_8Bit(0x1B);
//		datay[2] = SPI_Receive_8Bit();
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
//	
//		tempy=((((datay[0]<<16)|(datay[1]<<8)|(datay[2]&0xF0))<<8)>>12)*FACTOR_40;
//		
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
//	    SPI_Send_8Bit(0x1D);
//		dataz[0] = SPI_Receive_8Bit();
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
//		
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
//		SPI_Send_8Bit(0x1F);
//		dataz[1] = SPI_Receive_8Bit();
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
//		
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
//		SPI_Send_8Bit(0x21);
//		dataz[2] = SPI_Receive_8Bit();
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
//		
//		tempz=((((dataz[0]<<16)|(dataz[1]<<8)|(dataz[2]&0xF0))<<8)>>12)*FACTOR_40;
////        RS485_TX_EN;
////		printf("accx=%d   accy=%d  accz=%d\r\n", tempx,tempy,tempz);
////        HAL_Delay(500);
		if(RcvPacketFlag == 1)													// ���յ�������Ϣ֡��־
		{
				RcvPacketFlag = 0;													// ��ս��յ�������Ϣ֡��־
					 
				if(FrameCheck())													// ֡У����
					continue;
				
				MDB_Function();														// ���ܴ����� �����Ӧ��Ϣ֡
		 }
		

	}
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage 
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks 
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_LPUART1;
  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin : PA1 */
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : PB0->Ƭѡ */
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	/*Configure GPIO pin : PA7->SPI���� */
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : PA6->SPI��� */
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
}



void DataTransform(void)														// ��ASCIIģʽ��������ת����RTUģʽ ȥ����֡ͷ֡β��У������
{
	uint16_t i = 0;
	uint16_t Len = 0;
	
	Len = (RcvPacketLen - 3) / 2;												// ȥ��֡ͷ(1�ֽ�) ֡β(2�ֽ�) ��3�ֽ�  �ٳ���2(ASCIIģʽ2�ֽڱ�ʾ1���ֽ�����)
	
	for(i = 1; i < RcvPacketLen - 2; i++)										// ���ַ�0~9��A~Fת���ɶ�Ӧ��ֵ
	{
		if(master_request_data[i] >= '0' && master_request_data[i] <= '9')
			master_request_data[i] = master_request_data[i] - '0';
		else
		{
			if(master_request_data[i] >= 'A' && master_request_data[i] <= 'F')
				master_request_data[i] = master_request_data[i] - 'A' + 10;
		}
	}
	
	for(i = 0; i < Len; i++)													// ÿ2�ֽ����ݽ��кϲ���1�ֽ�����
		master_request_data[i] = (master_request_data[(i << 1) + 1] << 4) + 
								  master_request_data[(i << 1) + 2];
}

void FlashParaInit(void)														// Flash������ʼ��
{
  	uint16_t FlashWriteFlag;
	
	FlashWriteFlag = FlashReadHalfWord(CUSTOMER_FLAG_ADDR);						// ��Flashд��־λ
	
	if(FlashWriteFlag != FLAG_ENABLE)											// ��Flash��û����ʼ��
	{
		DefaultParaInit();														// ��Ĭ�ϲ������ڵ�ǰ����
	    WriteCurParaToFlash();													// �ѵ�ǰ����д��Flash��ǰ������
		BackupCurParaToFlash();													// �ѵ�ǰ����д��Flash���ݲ�����
	}
	
	else
	 ReadFlashPara();														// �����Flash�������ڵ����ݶ�����ǰ����
	
//	ParaCheck();																// ������� ����ֵ������ ���������ֵ����ΪĬ��ֵ(���ݲ�������ֵ)
}

void DefaultParaInit(void)														// ��Ĭ�ϲ������ڵ�ǰ����
{
	FlashPara.Common.UpgradeFlag 		= UPGRADE_DISABLE;						// ͨ�ò�����ʼ��
	FlashPara.Common.Addr		 		= (uint8_t)Default_CustomerPara.Addr;
	FlashPara.Common.Baud				= (uint8_t)Default_CustomerPara.Baud;
	FlashPara.Common.Parity		 		= (uint8_t)Default_CustomerPara.Parity;
	FlashPara.Common.OutPutMod	 		= (uint8_t)Default_CustomerPara.OutPutMod;

	FlashPara.Customer.Flag				= Default_CustomerPara.Flag;			// �ͻ�������ʼ��

	FlashPara.Customer.TempH			= Default_CustomerPara.TempH;
	FlashPara.Customer.TempL			= Default_CustomerPara.TempL;
	FlashPara.Customer.Addr				= Default_CustomerPara.Addr;
	FlashPara.Customer.Baud				= Default_CustomerPara.Baud;
	FlashPara.Customer.Parity			= Default_CustomerPara.Parity;
	FlashPara.Customer.Compensation		= Default_CustomerPara.Compensation;
    FlashPara.Customer.UploadFreq		= Default_CustomerPara.UploadFreq;
	FlashPara.Customer.Reset			= Default_CustomerPara.Reset;
	FlashPara.Customer.OutPutMod		= Default_CustomerPara.OutPutMod;
	FlashPara.Customer.Reserved[0]		= Default_CustomerPara.Reserved[0];
	FlashPara.Customer.Reserved[1]		= Default_CustomerPara.Reserved[1];		// ��������ڽṹ�嶨��ʱ�ѳ�ʼ��Ϊ��
}


void ReadFlashPara(void)														// ��Flash���ڵ����ݶ�����ǰ����
{
	DIS_INT;
	FlashReadNHalfWord(FLASH_BASE_ADDR, (uint16_t *)&FlashPara, 
					   sizeof(FlashPara_Typedef) >> 1);							// ��Flash������ȡ����ǰ�����ṹ����
	EN_INT;
}

void ParaCheck(void)															// ������� ����ֵ������ ���������ֵ����ΪĬ��Ϊ(��������������ֵ)
{
	uint16_t OverflowFlag = 0;													// ���������־

	if(FlashPara.Customer.Addr == 0 || FlashPara.Customer.Addr > 247)			// ��������ַ���
	{
		FlashPara.Customer.Addr = FlashReadHalfWord(CUSTOMER_SLAVE_ADDR - STANDBY_OFFSET_ADDR);
		FlashPara.Common.Addr = FlashPara.Customer.Addr;
		OverflowFlag = 1;
	}

	if(FlashPara.Customer.Baud == 0 || FlashPara.Customer.Baud > 7)				// �����������ʼ��
	{
		FlashPara.Customer.Baud = FlashReadHalfWord(CUSTOMER_BAUD_ADDR - STANDBY_OFFSET_ADDR);
		FlashPara.Common.Baud = FlashPara.Customer.Baud;
		OverflowFlag = 1;
	}

	if(FlashPara.Customer.Parity == 0 || FlashPara.Customer.Parity > 3)			// ��������żУ����
	{
		FlashPara.Customer.Parity = FlashReadHalfWord(CUSTOMER_PARITY_ADDR - STANDBY_OFFSET_ADDR);
		FlashPara.Common.Parity = FlashPara.Customer.Parity;
		OverflowFlag = 1;
	}

	if(FlashPara.Customer.OutPutMod == 0 || FlashPara.Customer.OutPutMod > 2)	// ���������ģʽ���
	{
		FlashPara.Customer.OutPutMod = FlashReadHalfWord(CUSTOMER_OUTPUTMOD_ADDR - STANDBY_OFFSET_ADDR);
		FlashPara.Common.OutPutMod = FlashPara.Customer.OutPutMod;
		OverflowFlag = 1;
	}

	if(FlashPara.Customer.Compensation == 0 || FlashPara.Customer.Compensation > 2)	// ����������ʹ�ܼ��
	{
		FlashPara.Customer.Compensation = FlashReadHalfWord(CUSTOMER_COMPENSATION_ADDR - STANDBY_OFFSET_ADDR);
		OverflowFlag = 1;
	}
  
	
	if(OverflowFlag)															// ������������ ���½���������
		WriteCurParaToFlash();													// �ѵ�ǰ����д��Flash��ǰ������
}

void ParaInit(void)															// ����Flash���� ������������ֵ
{		
//	PosParaInit();																// λ�Ʋ�����ʼ��
//	
//	MultinomialCoefInit();														// ����ʽϵ����ʼ��
//	
//	LogicalZeroInit();															// �߼�����ʼ��
//	
//	TempParaInit();																// �¶Ȳ�����ʼ��
	
	SerialPortParaInit();														// ���ڲ�����ʼ��
	
//	SamplingParaInit();															// ����������ʼ��
}

void SerialPortParaInit(void)													// ͨ�Ų�����ʼ��
{
	Out_Put_Mode = FlashPara.Customer.OutPutMod;			  // ��CustomerPara.OutPutMod���¶���һ���±��� ��ֹд���ģʽ����ʱ �������ģʽ�������� ���Ǳ��η�����ɺ�����Ч
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
