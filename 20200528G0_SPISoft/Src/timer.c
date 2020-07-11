#include "timer.h"
#include "main.h"

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
uint8_t Tim1Flag = 0;
uint8_t Tim2Flag = 1;															// Tim2��ʱ��־ ����

extern uint16_t SamplingFlag;													// ������ɱ�־ ��500��������ɺ� ��TIM4�ж��л���Ϊ1
extern uint16_t SamplingCount;													// ��������

extern float Volt;							

extern uint8_t RecPos;
extern uint8_t RcvByteFlag;
extern uint8_t RcvPacketLen;
extern uint8_t RcvPacketFlag;
extern uint8_t RecBuf[DataPacketLen];
extern FlashPara_Typedef FlashPara;												// Flash��������ṹ��
extern uint8_t master_request_data[DataPacketLen];
extern uint16_t Cycle3T5Byte[8];												// ��ͬ������3.5�ֽڶ�Ӧʱ��(ÿ�ֽڰ�11λ����) ��λ��us(����)
	
void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */
  
	
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  
	__HAL_RCC_TIM1_CLK_ENABLE();
  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 63999;         //1Khz
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 999;              //1s
  if(FlashPara.Customer.UploadFreq >= 1 && FlashPara.Customer.UploadFreq <7)
  htim1.Init.Period = FreTable[FlashPara.Customer.UploadFreq];	
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
	
	__HAL_TIM_CLEAR_IT(&htim1, TIM_IT_UPDATE);
	
	HAL_NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
	HAL_TIM_Base_Start_IT(&htim1);
	HAL_TIM_Base_Start(&htim1);
	if(FlashPara.Customer.UploadFreq==7)
	{
		HAL_TIM_Base_Stop(&htim1);
	}
}


void MX_TIM3_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    
	__HAL_RCC_TIM3_CLK_ENABLE();												      // Tim3ʱ��ʹ��
	
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 15;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	
	  htim3.Init.Period = Cycle3T5Byte[BAUDRATA_9600] - 1;						// Ĭ�ϰ�9600��������ʱ4.01ms
	  if(FlashPara.Customer.Baud >= 1 && FlashPara.Customer.Baud <= 7)
		htim3.Init.Period = Cycle3T5Byte[FlashPara.Customer.Baud] - 1;			// ��ͬ������ 3.5T��ʱ��ͬʱ�� �������ʱ����ΪĬ��ֵ
	
    if (HAL_TIM_Base_Init(&htim3) != HAL_OK)				                    // Tim3ʱ��ʹ��
    {
        Error_Handler();
    }
      sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
	  {
		Error_Handler();
	  }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    
    __HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE);
	
	  HAL_NVIC_SetPriority(TIM3_IRQn, 2, 1);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
    HAL_TIM_Base_Start_IT(&htim3);
    HAL_TIM_Base_Stop(&htim3);
}


void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
	static uint16_t Index = 0;

	if(__HAL_TIM_GET_FLAG(&htim1, TIM_FLAG_UPDATE) != RESET)
  {
		__HAL_TIM_CLEAR_IT(&htim1, TIM_IT_UPDATE);                              // �жϱ�־λ����
	    Tim1Flag=1;
//		PosSampling.DataArray[Index] = HAL_SDADC_InjectedGetValue(&hsdadc1, &channel) + 32768;	// ����ADֵȡֵΪ-32768~32767 ��32768ʹ��Χ���0~65535
//		TempSampling.DataArray[Index] = HAL_ADC_GetValue(&hadc1);				// ������ֵ��������
   
		if(++Index >= SamplingCount)											// ����������� ��ͷ���� ��Ҫд���� ���Ϊ���ڻ���� ������Ƶ�ʲ����仯ʱ ���������ᷢ���仯
		{
			Index = 0;
			SamplingFlag = SamplingFinish;
		}
	}
}



void TIM3_IRQHandler(void)
{
	if(__HAL_TIM_GET_FLAG(&htim3, TIM_FLAG_UPDATE) != RESET)
    {
		__HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE);                              // �жϱ�־λ��
		
		HAL_TIM_Base_Stop(&htim3);												// ֹͣ��ʱ��
		TIM3->CNT = 0;
	    
		if(RcvByteFlag == 1)                                                	// �����ڽ��յ�����
		{
			RcvPacketLen = RecPos;
			RcvByteFlag = 0;
			RcvPacketFlag = 1;
			MDB_Get_master_request_data(master_request_data, RecBuf, RcvPacketLen);		// �Ӵ������ݻ�������ȡ������ѯ��Ϣ֡����
		}
        
		RecPos = 0;
	}
}

