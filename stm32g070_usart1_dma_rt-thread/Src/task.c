/**@file        task.c
* @brief        任务模块
* @details      创建任务与执行任务
* @author       马灿林
* @date         2020-08-14
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


#include "task.h"
#include "stdio.h"
#include "flash.h"
#include "usart.h"

static struct rt_thread task1_thread;			 	//定义线程控制块 
static rt_uint8_t rt_task1_thread_stack[1024];		//定义线程栈

static struct rt_thread task2_thread;				//定义线程控制块 
static rt_uint8_t rt_task2_thread_stack[1024];		//定义线程栈


//函数声明
static void task1_thread_entry(void* parameter);
static void task2_thread_entry(void* parameter);


/**@brief       任务的初始化
* @return       函数执行结果1
*/
uint8_t task_init(void) 
{
	HAL_UART_Receive_DMA(&huart1, UsartType.RX_pData, RX_LEN);  
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE); 
	FlashPageErase(FLASH_BASE_ADDRESS);
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
	
	return 1;
}

/**@brief       线程1
* @return       无
*/
static void task1_thread_entry(void* parameter)
{
	uint32_t i=0,j = 0;
	while(1)
	{
		
		if(UsartType.RX_flag)    	
		{  
			UsartType.RX_flag=0;	
			printf("size:%d\n",UsartType.RX_Size);
			
			while(UsartType.RX_Size%8 != 0)
			{	
				UsartType.RX_pData[UsartType.RX_Size++] =0xFF;
			}
			FlashWriteDoubleWord(FLASH_BASE_ADDRESS+i, (uint64_t*)UsartType.RX_pData,UsartType.RX_Size/8);	
			i=i+(UsartType.RX_Size);
		}
		if(j == 100)
		{
			j = 0;
			printf("task1\n");
		}
		j++;
		rt_thread_mdelay(5);/* 延时500个tick */		
	}
}

/**@brief       线程2
* @return       无
*/
static void task2_thread_entry(void* parameter)
{	
	while(1)
	{
		printf("task2\n");
		rt_thread_mdelay(500);/* 延时500个tick */	
	}
}


