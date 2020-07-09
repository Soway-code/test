/**
 * 版权：   
 * 文件名:  main.c
 * 作者:    
 * 版本:    
 * 日期:    2020-07-02
 * 描述:    
 * 功能:         

 * 修改者:      No
 * 版本:  		
 * 修改内容:    No 
 * 日期:        No
 */
#include "main.h"


static struct rt_thread task1_thread;			 	//定义线程控制块 
static rt_uint8_t rt_task1_thread_stack[1024];		//定义线程栈



static struct rt_thread task2_thread;				 //定义线程控制块 
static rt_uint8_t rt_task2_thread_stack[1024];		//定义线程栈


//函数声明
static void task1_thread_entry(void* parameter);
static void task2_thread_entry(void* parameter);

void SystemClock_Config(void);


/**
 * 名称         : main()
 * 创建日期     : 
 * 作者         : 
 * 功能         : main函数
 * 输入参数     : 无
 * 输出参数     : 无
 * 返回结果     : 无
 * 注意和说明   : 无
 * 修改内容     : 无 
 */
int main(void)
{

	HAL_Init();

	SystemClock_Config();
	
	MX_GPIO_Init();
 
	
	rt_thread_init(&task1_thread, 						//线程控制块 
					"task1", 							//线程名字 
					task1_thread_entry, 				// 线程入口函数 
					RT_NULL, 							// 线程入口函数参数 
					&rt_task1_thread_stack[0],			// 线程栈起始地址 
					sizeof(rt_task1_thread_stack), 		// 线程栈大小 
					3, 									// 线程的优先级 
					20); 								// 线程时间片 
	rt_thread_startup(&task1_thread); 					// 启动线程，开启调度 
	
	
	
	rt_thread_init(&task2_thread, 						// 线程控制块 
					"task2", 							// 线程名字 
					task2_thread_entry, 				// 线程入口函数 
					RT_NULL, 							// 线程入口函数参数 
					&rt_task2_thread_stack[0],			// 线程栈起始地址 
					sizeof(rt_task2_thread_stack), 		// 线程栈大小 
					3, 									// 线程的优先级 
					20); 								// 线程时间片 
	rt_thread_startup(&task2_thread); 					// 启动线程，开启调度 
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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


/**
 * 名称         : task1_thread_entry(void* parameter)
 * 创建日期     : 
 * 作者         : 
 * 功能         : 线程1
 * 输入参数     : 无
 * 输出参数     : 无
 * 返回结果     : 无
 * 注意和说明   : 无
 * 修改内容     : 无 
 */
static void task1_thread_entry(void* parameter)
{
	while(1)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
		rt_thread_delay(500);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
		rt_thread_delay(500);
	}
}


/**
 * 名称         : task2_thread_entry(void* parameter)
 * 创建日期     : 
 * 作者         : 
 * 功能         : 线程2
 * 输入参数     : 无
 * 输出参数     : 无
 * 返回结果     : 无
 * 注意和说明   : 无
 * 修改内容     : 无 
 */
static void task2_thread_entry(void* parameter)
{
	while(1)
	{
		
	}
}
