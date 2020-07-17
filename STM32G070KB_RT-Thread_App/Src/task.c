/**@file      	task.c
* @brief     	Threaded programs
* @default		Function thread function entry
* @author		loodao any question please send mail to 893204847@qq.com
* @date 		2020-07-17
* @version		V1.0
* @copyright	
************************************************************************
* @attention
* @brief hardware platefrom:stm32G070KB
* @brief OS Version: RT-Thread 3.1.3
* @brief para modify log:
* <table>
* <tr><th>Date	<th>Version 
* <tr><td>2020/07/08 <td>1.0 <td>Loodao <td> buidling new version
* </table>
*
***********************************************************************
*/

#include "task.h"
#include<string.h>

uint8_t UART1_RXBuff[200];
uint8_t buf[16]={":FF4100010000BF"};
/// 定义线程控制块指针 
static rt_thread_t task1_18B20 = RT_NULL;
static rt_thread_t task2_OLED = RT_NULL;
static rt_thread_t task3_usart = RT_NULL;

/// 函数声明
static void task1_18B20_entry(void* parameter);
static void task2_OLED_entry(void* parameter);
static void task3_usart_entry(void* parameter);
uint32_t temperature; 

/// 定义信号量控制块
static rt_sem_t test_sem = RT_NULL;


/**
 * 名称         : uint8_t task_init(void)	
 * 创建日期     : 2020-07-17
 * 作者         : 
 * 功能         : 初始化 创建信号量,线程
 * 输入参数     : 无
 * 输出参数     : 无
 * 返回结果     : 1 创建成功  0 创建失败
 * 注意和说明   : 无
 * 修改内容     : 无 
 */
uint8_t task_init(void)
{
	///创建一个信号量
	test_sem = rt_sem_create("test_sem",	///信号量名字 
                     1,     				///信号量初始值，默认有一个信号量
                     RT_IPC_FLAG_FIFO); 	///信号量模式 FIFO(0x00)
	
	task1_18B20 =                          ///线程控制块指针 
    rt_thread_create( "task1_18B20",       /// 线程名字 
                       task1_18B20_entry,  /// 线程入口函数 
					   RT_NULL,            /// 线程入口函数参数 
                       512,                /// 线程栈大小 
                       4,                  /// 线程的优先级 
                       20);                /// 线程时间片 
	 /* 启动线程，开启调度 */
	if (task1_18B20 != RT_NULL)
        rt_thread_startup(task1_18B20);
	else
       return 0;
    
	
    task2_OLED =                          	/// 线程控制块指针 
    rt_thread_create( "task2_OLED",         /// 线程名字 
                       task2_OLED_entry,  	/// 线程入口函数 
                       RT_NULL,            	/// 线程入口函数参数 
                       512,                	/// 线程栈大小 
                       4,                   /// 线程的优先级 
                       20);                	/// 线程时间片 
                   
    ///启动线程，开启调度 
	if (task2_OLED != RT_NULL)
        rt_thread_startup(task2_OLED);
    else
        return 0; 
	
	task3_usart =                          	/// 线程控制块指针 
    rt_thread_create( "task3_usart",        /// 线程名字 
                       task3_usart_entry,  	/// 线程入口函数 
                       RT_NULL,            	/// 线程入口函数参数 
                       512,                	/// 线程栈大小 
                       4,                   /// 线程的优先级 
                       20);                	/// 线程时间片 
                   
    ///启动线程，开启调度 
	if (task3_usart != RT_NULL)
        rt_thread_startup(task3_usart);
    else
        return 0; 
	
	return 1; 

}


/**
 * 名称         : task1_18B20_entry(void* parameter)
 * 创建日期     : 
 * 作者         : 
 * 功能         : 线程1
 * 输入参数     : 无
 * 输出参数     : 无
 * 返回结果     : 无
 * 注意和说明   : 无
 * 修改内容     : 无 
 */
static void task1_18B20_entry(void* parameter)
{
	
	while(1)
	{
		rt_sem_take(test_sem,			/// 获取信号量 
                RT_WAITING_FOREVER); 	/// 等待时间：一直等 
		
		temperature=DS18B20_Get_Temp();	
		
		rt_sem_release(	test_sem );		///释放二值信号量
		
		printf("%d\n",temperature);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
		rt_thread_delay(500);
		
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
		rt_thread_delay(500);
	}
}

/**
 * 名称         : task2_OLED_entry(void* parameter)
 * 创建日期     : 
 * 作者         : 
 * 功能         : 线程2
 * 输入参数     : 无
 * 输出参数     : 无
 * 返回结果     : 无
 * 注意和说明   : 无
 * 修改内容     : 无 
 */
static void task2_OLED_entry(void* parameter)
{

	uint8_t buf[5];
	OLED_Init();				/// 初始化OLED 
	OLED_Clear(); 				/// 清除屏幕
	OLED_ShowCHinese(0,0,0);	///
	OLED_ShowCHinese(16,0,1);	///
	OLED_ShowCHinese(32,0,2);	///
	OLED_ShowCHinese(48,0,3);	///
	OLED_ShowCHinese(64,0,4);	///
	OLED_ShowCHinese(112,0,5);	///
	while(1)
	{
		sprintf((char*)buf,"%d.%d",temperature/10,(temperature%10));
		
		rt_sem_take(test_sem,			/// 获取信号量 
                RT_WAITING_FOREVER); 	/// 等待时间：一直等 
		
		OLED_ShowString(80,0,buf,16);
		
		rt_sem_release(	test_sem );		/// 释放二值信号量
		
		if(850 == temperature)
		{
			OLED_ShowString(0,2,"Ds18b20Error!",16);
			while(850 == temperature)
			{
				rt_thread_delay(10);
			}
			OLED_ShowString(0,2,"             ",16);
		}
		
		rt_thread_delay(500);
	}
}


/**
 * 名称         : task3_usart_entry(void* parameter)
 * 创建日期     : 
 * 作者         : 
 * 功能         : 线程3
 * 输入参数     : 无
 * 输出参数     : 无
 * 返回结果     : 无
 * 注意和说明   : 无
 * 修改内容     : 无 
 */
uint32_t i=0;
static void task3_usart_entry(void* parameter)
{
	while(1)
	{
		
		 if (HAL_UART_Receive_IT(&huart1, (uint8_t *)UART1_RXBuff, 50) != HAL_OK)
		{
			
		}
		
		if('\0' != UART1_RXBuff[0])
		{
			printf("%s\n",UART1_RXBuff);
			
			if(0 == strncmp((char*)UART1_RXBuff,(char*)buf,15))		//升级指令一致
				HAL_NVIC_SystemReset();								///软复位

		}
	
	
//		if (HAL_UART_Transmit_IT(&huart1, (uint8_t *)UART1_RXBuff, 200) != HAL_OK)
//  	{
//	  
//		 	 printf("HAL_UART_Transmit_IT\n");
//   		// Error_Handler();
//  	}
	
		rt_thread_delay(500);
	}
}

