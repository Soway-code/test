/**@file        tim.c
* @brief        tim模块的应用
* @details      定时器功能配置 定时器延时函数
* @author       马灿林
* @date         2020-08-01
* @version      V1.0.0
* @copyright    2020-2030,深圳市信为科技发展有限公司
**********************************************************************************
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/08/01  <td>1.0.0    <td>马灿林    <td>创建初始版本
* </table>
*
**********************************************************************************
*/

#include "tim.h"

	//默认32Mhz
	uint8_t	PRES 		=	2;		///预分频系数
	uint8_t	CNT_1us		=	1;
	uint8_t	CNT_2us		=	2;
	uint8_t	CNT_5us		=	36;
	uint16_t CNT_15us	=	144;
	uint16_t CNT_60us	=	624;
	uint16_t CNT_750us	=	7980;
	uint16_t CNT_1000us	=	10648;
#if USE_TIMER_X != 14
	TIM_HandleTypeDef htim3;
#else
	TIM_HandleTypeDef htim14;
#endif

/**
* @brief 初始化定时器
*/
void MX_TIMx_Init(void)
{
#if USE_TIMER_X != 14
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
	
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = PRES;					//预分频系数
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;					//重装载值
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
	//Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
	//Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
   // Error_Handler();
  }
  
#else  

  htim14.Instance = TIM14;
  htim14.Init.Prescaler = PRES;		//预分频系数
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 65535;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }

#endif
  
}


void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{
#if USE_TIMER_X != 14	
  if(tim_baseHandle->Instance==TIM3)
  {
	  
    __HAL_RCC_TIM3_CLK_ENABLE();
	  
  }
#endif 
#if USE_TIMER_X == 14
	if(tim_baseHandle->Instance==TIM14)
  {
 
    __HAL_RCC_TIM14_CLK_ENABLE();

  }
#endif 
}


void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{
#if USE_TIMER_X != 14
  if(tim_baseHandle->Instance==TIM3)
  {
	  
    __HAL_RCC_TIM3_CLK_DISABLE();

  }
#endif 
#if USE_TIMER_X == 14
	if(tim_baseHandle->Instance==TIM14)
  {

    __HAL_RCC_TIM14_CLK_DISABLE();

  }
#endif 
} 

/**
* @brief 1微秒延时函数
*/
void delay_tim_1us()
{
#if USE_TIMER_X != 14	
	
  (&htim3)->Instance->CNT = (0x0000efff - CNT_1us);//  __HAL_TIM_SET_COUNTER(&htim3,differ);
 
//  HAL_TIM_Base_Start(&htim3);
  SET_BIT(TIM3->CR1, TIM_CR1_CEN);
 
//__HAL_TIM_GET_COUNTER(&htim3);
 while(((&htim3)->Instance->CNT) < 0x0000effe)	//  READ_REG(TIM3->CNT) 
  {
 
  }
//  HAL_TIM_Base_Stop(&htim3);
  CLEAR_BIT(TIM3->CR1, TIM_CR1_CEN);

#else 
	
	(&htim14)->Instance->CNT = (0x0000efff - CNT_1us);
	SET_BIT(TIM14->CR1, TIM_CR1_CEN);
 
	while(((&htim14)->Instance->CNT) < 0x0000effe)
	{
 
	}

	CLEAR_BIT(TIM14->CR1, TIM_CR1_CEN);
	
#endif
	
}


/**
* @brief 2微秒延时函数
*/
void delay_tim_2us()
{	
#if USE_TIMER_X != 14 
	
  (&htim3)->Instance->CNT = (0x0000efff - CNT_2us);//  __HAL_TIM_SET_COUNTER(&htim3,differ);
//  HAL_TIM_Base_Start(&htim3);
  SET_BIT(TIM3->CR1, TIM_CR1_CEN);
 
//__HAL_TIM_GET_COUNTER(&htim3);
  while(((&htim3)->Instance->CNT) < 0x0000effe)	//  READ_REG(TIM3->CNT) 
  {
 
  }
//  HAL_TIM_Base_Stop(&htim3);
  CLEAR_BIT(TIM3->CR1, TIM_CR1_CEN);
	
#else 

	(&htim14)->Instance->CNT = (0x0000efff - CNT_2us);
	SET_BIT(TIM14->CR1, TIM_CR1_CEN);
 
	while(((&htim14)->Instance->CNT) < 0x0000effe)
	{
 
	}

	CLEAR_BIT(TIM14->CR1, TIM_CR1_CEN);
	
#endif
	
}

/**
* @brief 5微秒延时函数
*/
void delay_tim_5us()
{
#if USE_TIMER_X != 14 
	
  (&htim3)->Instance->CNT = (0x0000efff - CNT_5us);//  __HAL_TIM_SET_COUNTER(&htim3,differ);
 
//  HAL_TIM_Base_Start(&htim3);
  SET_BIT(TIM3->CR1, TIM_CR1_CEN);
 
//__HAL_TIM_GET_COUNTER(&htim3);
  while(((&htim3)->Instance->CNT) < 0x0000effe)	//  READ_REG(TIM3->CNT) 
  {
 
  }
//  HAL_TIM_Base_Stop(&htim3);
  CLEAR_BIT(TIM3->CR1, TIM_CR1_CEN);
  
#else 

	(&htim14)->Instance->CNT = (0x0000efff - CNT_5us);
	SET_BIT(TIM14->CR1, TIM_CR1_CEN);
 
	while(((&htim14)->Instance->CNT) < 0x0000effe)
	{
 
	}

	CLEAR_BIT(TIM14->CR1, TIM_CR1_CEN);
	
#endif
	
}

/**
* @brief 15微秒延时函数
*/
void delay_tim_15us()
{
#if USE_TIMER_X != 14
	
  (&htim3)->Instance->CNT = (0x0000efff -CNT_15us);//  __HAL_TIM_SET_COUNTER(&htim3,differ);
 
//  HAL_TIM_Base_Start(&htim3);
  SET_BIT(TIM3->CR1, TIM_CR1_CEN);
 
//__HAL_TIM_GET_COUNTER(&htim3);
  while(((&htim3)->Instance->CNT) < 0x0000effe)	//  READ_REG(TIM3->CNT) 
  {
 
  }
//  HAL_TIM_Base_Stop(&htim3);
  CLEAR_BIT(TIM3->CR1, TIM_CR1_CEN);
 
#else 

	(&htim14)->Instance->CNT = (0x0000efff - CNT_15us);
	SET_BIT(TIM14->CR1, TIM_CR1_CEN);
 
	while(((&htim14)->Instance->CNT) < 0x0000effe)
	{
 
	}

	CLEAR_BIT(TIM14->CR1, TIM_CR1_CEN);
	
#endif
}

/**
* @brief 60微秒延时函数
*/
void delay_tim_60us()
{
#if USE_TIMER_X != 14
	
  (&htim3)->Instance->CNT = (0x0000efff -CNT_60us);//  __HAL_TIM_SET_COUNTER(&htim3,differ);
 
//  HAL_TIM_Base_Start(&htim3);
  SET_BIT(TIM3->CR1, TIM_CR1_CEN);
 
//__HAL_TIM_GET_COUNTER(&htim3);
  while(((&htim3)->Instance->CNT) < 0x0000effe)	//  READ_REG(TIM3->CNT) 
  {
 
  }
//  HAL_TIM_Base_Stop(&htim3);
  CLEAR_BIT(TIM3->CR1, TIM_CR1_CEN);
 
#else 

	(&htim14)->Instance->CNT = (0x0000efff - CNT_60us);
	SET_BIT(TIM14->CR1, TIM_CR1_CEN);
 
	while(((&htim14)->Instance->CNT) < 0x0000effe)
	{
 
	}

	CLEAR_BIT(TIM14->CR1, TIM_CR1_CEN);
	
#endif
	
}

/**
* @brief 750微秒延时函数
*/
void delay_tim_750us()
{
#if USE_TIMER_X != 14
	
  (&htim3)->Instance->CNT = (0x0000efff -CNT_750us);//  __HAL_TIM_SET_COUNTER(&htim3,differ);
 
//  HAL_TIM_Base_Start(&htim3);
  SET_BIT(TIM3->CR1, TIM_CR1_CEN);
 
//__HAL_TIM_GET_COUNTER(&htim3);
  while(((&htim3)->Instance->CNT) < 0x0000effe)	//  READ_REG(TIM3->CNT) 
  {
 
  }
//  HAL_TIM_Base_Stop(&htim3);
  CLEAR_BIT(TIM3->CR1, TIM_CR1_CEN);
  
#else 

	(&htim14)->Instance->CNT = (0x0000efff - CNT_750us);
	SET_BIT(TIM14->CR1, TIM_CR1_CEN);
 
	while(((&htim14)->Instance->CNT) < 0x0000effe)
	{
 
	}

	CLEAR_BIT(TIM14->CR1, TIM_CR1_CEN);
	
#endif
	
}

/**
* @brief 1000微秒延时函数
*/
void delay_tim_1000us()
{
#if USE_TIMER_X != 14
	
  (&htim3)->Instance->CNT = (0x0000efff - CNT_1000us);//  __HAL_TIM_SET_COUNTER(&htim3,differ);
 
//  HAL_TIM_Base_Start(&htim3);
  SET_BIT(TIM3->CR1, TIM_CR1_CEN);
 
//__HAL_TIM_GET_COUNTER(&htim3);
  while(((&htim3)->Instance->CNT) < 0x0000effe)	//  READ_REG(TIM3->CNT) 
  {
 
  }
//  HAL_TIM_Base_Stop(&htim3);
  CLEAR_BIT(TIM3->CR1, TIM_CR1_CEN);
  
#else 

	(&htim14)->Instance->CNT = (0x0000efff - CNT_1000us);
	SET_BIT(TIM14->CR1, TIM_CR1_CEN);
 
	while(((&htim14)->Instance->CNT) < 0x0000effe)
	{
 
	}

	CLEAR_BIT(TIM14->CR1, TIM_CR1_CEN);
	
#endif
	
}

/**@brief       毫秒延时函数
* @param[in]    ms : 毫秒延时时间
* - None
* @note         引用ds18b20.h 头文件既可以使用该函数
*/
void delay_tim_ms(uint32_t ms)
{
	while(ms--)
	{
		delay_tim_1000us();
	}
}

/**@brief       秒延时函数
* @param[in]    s : 秒延时时间
* - None
* @note         引用ds18b20.h 头文件既可以使用该函数
*/
void delay_tim_s(uint32_t s)
{
	while(s--)
	{
		delay_tim_ms(1000);
	}
}

/**
* @brief 获取HCLK时钟频率
* @return 返回值：HCLK时钟频率
* @retval 获取HCLK时钟频率失败返回 0
*/
uint8_t Get_HCLKFreq()
{
	if( ((22 <= (SystemCoreClock/1000000)) && (26 >= (SystemCoreClock/1000000))) || (24 == HclkFrequency) )
	{
		//24Mhz
		PRES 		= 2;		///预分频系数
		CNT_1us		= 1;
		CNT_2us		= 2;
		CNT_5us		= 20;
		CNT_15us	= 102;
		CNT_60us	= 460;
		CNT_750us	= 5980;
		CNT_1000us  = 7976;
		return 24;
	}
	else if( ((30 <= (SystemCoreClock/1000000)) && (34 >= (SystemCoreClock/1000000))) || (32 == HclkFrequency) )
	{
		//32Mhz
		PRES 		= 2;		///预分频系数
		CNT_1us		= 1;
		CNT_2us		= 2;
		CNT_5us		= 36;
		CNT_15us	= 140;
		CNT_60us	= 624;
		CNT_750us	= 7980;
		CNT_1000us  = 10648;
		return 32;
	}
	else if( ((46 <= (SystemCoreClock/1000000)) && (50 >= (SystemCoreClock/1000000))) || (48 == HclkFrequency) )
	{
		//48Mhz
		PRES		= 6;		///预分频系数
		CNT_1us		= 2;
		CNT_2us		= 8;
		CNT_5us		= 29;
		CNT_15us	= 96;
		CNT_60us	= 408;
		CNT_750us	= 5134;
		CNT_1000us  = 6850;
		return 48;
	}
	else if( ((62 <= (SystemCoreClock/1000000)) && (66 >= (SystemCoreClock/1000000))) || (64 == HclkFrequency) )
	{
		//64Mhz
		PRES		= 8;		///预分频系数
		CNT_1us		= 2;
		CNT_2us		= 8;
		CNT_5us		= 29;
		CNT_15us	= 98;
		CNT_60us	= 420;
		CNT_750us	= 5310;
		CNT_1000us  = 7108;
		return 64;
	}
	else if( ((70 <= (SystemCoreClock/1000000)) && (74 >= (SystemCoreClock/1000000))) || (72 == HclkFrequency) )
	{
		//72Mhz
		PRES		= 9;		///预分频系数
		CNT_1us		= 3;
		CNT_2us		= 10;
		CNT_5us		= 31;
		CNT_15us	= 104;
		CNT_60us	= 430;
		CNT_750us	= 5396;
		CNT_1000us  = 7198;
		return 72;
	}
	else if(((88 <= (SystemCoreClock/1000000)) && (170 >= (SystemCoreClock/1000000))) || (168 == HclkFrequency) )
	{
		//168Mhz
		PRES		= 11;		///预分频系数	
		CNT_1us		= 5;
		CNT_2us		= 12;
		CNT_5us		= 34;
		CNT_15us	= 104;
		CNT_60us	= 423;
		CNT_750us	= 5246;
		CNT_1000us  = 7000;
		return 168;
	}
	
	return HclkFrequency; 
}

