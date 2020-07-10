/**
* @file         tim.c
* @brief        ��ʱ��Ӧ��
* @details      ��ʱ����ʼ�����жϴ���
* @author       weijianx
* @date         2020-07-02
* @version      V1.0.0
* @copyright    2020-2030,��������Ϊ�Ƽ���չ���޹�˾
**********************************************************************************
* @par �޸���־:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/07/02  <td>1.0.0    <td>weijianx  <td>������ʼ�汾
* </table>
*
**********************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "tim.h"
#include "type.h"


/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;

uint8_t Time_1s_flag = 0;
uint8_t Time_2s_flag = 0;
uint8_t Time_5s_flag = 0;
uint8_t Time_1min_flag = 0;
uint16_t tim_cnt = 0;   //��ʱ���ۼ�


uint16_t tim_1min_cnt = 0;   //1minʱ���ۼ�

extern uint16_t Current_pulse;  //��ǰ������

uint8_t first_20s = 0;   //�������ǰ20��


BitAction  PulseFlag = Bit_RESET;                                         //��ʼװ�������־
uint16_t Pulse100msCntBuf[3] = {0};                                     //ÿ100ms�������������
extern UserTypeDef UserPara;


/* TIM1 init function */
void MX_TIM1_Init(void)
{
   TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 0;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_ETRMODE2;
  sClockSourceConfig.ClockPolarity = TIM_CLOCKPOLARITY_NONINVERTED;
  sClockSourceConfig.ClockPrescaler = TIM_CLOCKPRESCALER_DIV1;
  sClockSourceConfig.ClockFilter = 0;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.BreakAFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.Break2AFMode = TIM_BREAK_AFMODE_INPUT;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }

  	HAL_TIM_Base_Start_IT(&htim1);

}

void TIM1_ETR_Init(void)   //�������������
{
    uint32_t tmpcr1 = 0U;
    uint32_t tmpsmcr = 0U;
	GPIO_InitTypeDef GPIO_InitStruct = {0};
//    SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOAEN);                                   //GPIOAʱ�ӿ���
//    SET_BIT(RCC->APB2ENR, (RCC_APB2ENR_TIM21EN));                                //TIM21ʱ�ӿ���
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_TIM1_CLK_ENABLE();
	
   /*
    GPIOA->MODER &= 0xFFCFFFFF;                                                 //PA10���ù���                                
    GPIOA->MODER |= 0x00200000;//10: Alternate function mode      
    GPIOA->OTYPER &= ~(1<<10);                                                    //�������
    GPIOA->OSPEEDR |= (3<<20);            //���� 
    GPIOA->PUPDR &= 0xFFCFFFFF;                                                 //���� 
    GPIOA->PUPDR |= 0x00200000; 
    GPIOA->AFR[1] &= 0xFFFFFFF0;                                                //AF5 TIM22_CH2
    GPIOA->AFR[1] |= 0x00000005;                                                //AFR[0]��ӦPIN0~7,AFR[1]��ӦPIN8~15
    */  
   
//    GPIOA->MODER &= 0xFFFFFFF3;                                                 //PA1���ù���                                
//    GPIOA->MODER |= 0x00000008;      
//    GPIOA->OTYPER &= ~(0x1<<1);                                                    //�������
//    GPIOA->OSPEEDR |= (0x3<<2);
//    GPIOA->PUPDR &= 0xFFFFFFF3;  //��Ҫ����Ϊ��������  �����Ƶ��׼                                             
//    GPIOA->PUPDR |= 0x00000000; // 00: No pull-up, pull-down     01: Pull-up     10: Pull-down
//    GPIOA->AFR[0] &= 0xFFFFFF0F;                                                //AF5 TIM21_ETR
//    GPIOA->AFR[0] |= 0x00000050;                                                //AFR[0]��ӦPIN0~7,AFR[1]��ӦPIN8~15
    
	    /**TIM1 GPIO Configuration    
    PA12 [PA10]     ------> TIM1_ETR 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
    tmpcr1  = TIM1->CR1;                                                         //TIM21 Base Init
    tmpcr1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);
    tmpcr1 |= TIM_COUNTERMODE_UP;
    tmpcr1 &= ~TIM_CR1_CKD;
    tmpcr1 |= 0x00;
    TIM1->CR1 = tmpcr1;
    TIM1->ARR = 0xFFFF;
    TIM1->PSC = 0x00;
    TIM1->EGR = TIM_EGR_UG;    
    
    tmpsmcr = TIM1->SMCR;                                                       //TIM21 ETR����
    tmpsmcr &= ~(TIM_SMCR_ETF | TIM_SMCR_ETPS | TIM_SMCR_ECE | TIM_SMCR_ETP);
    tmpsmcr |= (uint32_t)(TIM_ETRPRESCALER_DIV1 | (TIM_ETRPOLARITY_NONINVERTED | (0x0F << 8)));
    TIM1->SMCR = tmpsmcr;
    TIM1->SMCR |= TIM_SMCR_ECE;                                                 //ʹ���ⲿʱ��ģʽ2������Ҫ������
    
    
    TIM1->CNT = 0;                                                              //����ֵ��0
    TIM1->CR1|= TIM_CR1_CEN;                                                    //ʹ��TIM21
}



/* TIM3 init function */
void MX_TIM3_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 6399;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 9999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
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
  
  HAL_TIM_Base_Start_IT(&htim3); //ʹ�ܶ�ʱ��3�Ͷ�ʱ��3�����жϣ�TIM_IT_UPDATE 

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

//  if(tim_baseHandle->Instance==TIM1)
//  {
//  /* USER CODE BEGIN TIM1_MspInit 0 */

//  /* USER CODE END TIM1_MspInit 0 */
//    /* TIM1 clock enable */
//    __HAL_RCC_TIM1_CLK_ENABLE();

//    /* TIM1 interrupt Init */
//    HAL_NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 0, 0);
//    HAL_NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
//  /* USER CODE BEGIN TIM1_MspInit 1 */

//  /* USER CODE END TIM1_MspInit 1 */
//  }
  if(tim_baseHandle->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspInit 0 */

  /* USER CODE END TIM3_MspInit 0 */
    /* TIM3 clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();

    /* TIM3 interrupt Init */
    HAL_NVIC_SetPriority(TIM3_IRQn, 1, 3);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
  /* USER CODE BEGIN TIM3_MspInit 1 */

  /* USER CODE END TIM3_MspInit 1 */
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspDeInit 0 */

  /* USER CODE END TIM1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM1_CLK_DISABLE();

    /* TIM1 interrupt Deinit */
//    HAL_NVIC_DisableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
  /* USER CODE BEGIN TIM1_MspDeInit 1 */

  /* USER CODE END TIM1_MspDeInit 1 */
  }
  else if(tim_baseHandle->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspDeInit 0 */

  /* USER CODE END TIM3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM3_CLK_DISABLE();

    /* TIM3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM3_IRQn);
  /* USER CODE BEGIN TIM3_MspDeInit 1 */

  /* USER CODE END TIM3_MspDeInit 1 */
  }
} 


//void Tim3NvicInit(void)
//{
//	__HAL_RCC_TIM3_CLK_ENABLE();            //ʹ��TIM3ʱ��
//	HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
//	HAL_NVIC_EnableIRQ(TIM3_IRQn);

//}


//void TIM3_Init(void)
//{
//	MX_TIM3_Init();
//	Tim3NvicInit();
//	
//	__HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_UPDATE);
//	__HAL_TIM_ENABLE_IT(&htim3, TIM_IT_UPDATE);
//	__HAL_TIM_ENABLE(&htim3);

//}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	uint16_t read_pulse = 0;  //��ǰ������
	
	if(htim->Instance == TIM3)
	{
		tim_cnt++;
		
		if(tim_cnt % 2 == 0)
		{
			Time_2s_flag = 1;
		}
		
		tim_1min_cnt += 1;   //��һ��Ϊ1s

		read_pulse = TIM1->CNT;    //��ȡ��ǰ��������
		TIM1->CNT = 0;      


		UserPara.TotalPulse += read_pulse;  //��������                   


		if(read_pulse)
		{            
			PulseFlag = Bit_SET;  //������   ת����
			Time_1min_flag = 0;
		}
		else
		{
			 Time_1min_flag++;
			 
			if(Time_1min_flag > 180)
			{
				Time_1min_flag = 0;
				PulseFlag = Bit_RESET; //������  ͣת
			}
		}             
		  

		if(!((tim_cnt+1)%1))  //1s
		{
			Time_1s_flag = 1;

		}
		if(!((tim_cnt +1)%5))//5s��ʱ
		{
			Time_5s_flag = 1;
		}

		if((tim_cnt < 20) && (first_20s ==0) )
		{ 
			if(tim_cnt == 19)
				first_20s = 1;
			UserPara.RotateSpeed = read_pulse * 3;

		}                 
		else if (!((tim_cnt +1)%20))//20s��ʱ
		{            
			UserPara.RotateSpeed = (UserPara.TotalPulse - Current_pulse) * 3 ;  //������ת�ٶ�  1s������*60  HZ/min  ��λ��תÿ��
			Current_pulse = UserPara.TotalPulse;              
		}

	}

}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
