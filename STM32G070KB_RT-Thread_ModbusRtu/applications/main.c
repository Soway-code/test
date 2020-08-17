/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */


#include "task.h"

/* defined the LED pin: PB4 */
#define LED0_PIN     GET_PIN(B, 4)
#define USING_RT_THREAD_OS



int main(void)
{
    /// set LED0 pin mode to output 
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
	
	if(RT_ERROR == task_init())						///初始化创建线程
	{
		rt_kprintf("Thread creation failed!!!!!!\n");
		while(1) ;
	}
    return RT_EOK;
}
