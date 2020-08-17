/**@file        task.c
* @brief        任务模块
* @details      创建任务与执行任务
* @author       马灿林
* @date         2020-07-17
* @version      V1.0.0
* @copyright    2020-2030,深圳市信为科技发展有限公司
**********************************************************************************
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/07/17  <td>1.0.0    <td>马灿林    <td>创建初始版本
* </table>
*
**********************************************************************************
*/

#include "task.h"

///定义线程控制块指针 
static rt_thread_t task1_18B20 = RT_NULL;
static rt_thread_t task2_OLED = RT_NULL;


static rt_sem_t test_sem = RT_NULL;				/// 定义信号量控制块

uint32_t temperature;							//存放18B20温度

ModBusBaseParam_TypeDef     ModBusBaseParam;    //ModBus处理的基本参数结构
ModBus_Device_Param         ModBus_Device;  	//ModBus管理设备的参数结构体

/**
* @brief 任务函数声明
*/
static void task1_18B20_entry(void* parameter);
static void task2_OLED_entry(void* parameter);


/**@brief       任务的初始化函数
* @param[in]    无
* @param[out]   无
* @return       任务函数初始化结果
* - RT_EOK(0) (成功)
* - RT_ERROR(1)(失败)
*/
uint8_t task_init(void) 
{
	
	///创建一个信号量
	test_sem = 
	rt_sem_create("test_sem",	///信号量名字 
                     1,     				///信号量初始值，默认有一个信号量
                     RT_IPC_FLAG_FIFO); 	///信号量模式 FIFO(0x00)
	
	///创建线程
	task1_18B20 =                          	///线程控制块指针 
    rt_thread_create( "ds18b20",            ///线程名字 
                      task1_18B20_entry,   	///线程入口函数 
                      RT_NULL,            	///线程入口函数参数
                      512,                 	///线程栈大小
                      3,                   	///线程的优先级 
                      20);                 	///线程时间片 
                   

   if (task1_18B20 != RT_NULL)
        rt_thread_startup(task1_18B20);		///启动线程，开启调度 
    else
        return RT_ERROR;
    
	
    task2_OLED =                           ///线程控制块指针 
    rt_thread_create( "OLED",              ///线程名字  
                      task2_OLED_entry,    ///线程入口函数  
                      RT_NULL,             ///线程入口函数参数 
                      512,                 ///线程栈大小 
                      4,                   ///线程的优先级  
                      20);                 ///线程时间片  
                   
 
   if (task2_OLED != RT_NULL)
        rt_thread_startup(task2_OLED);		 ///启动线程，开启调度 
    else
        return RT_ERROR;
		
	return RT_EOK;
}


/**@brief       线程1
* @return       无
*/
static void task1_18B20_entry(void* parameter)
{	
	uint32_t i=0;
	DS18B20_Init();					///ds18b20传感器初始化
	//MX_TIM3_Init();
	while(1)
	{
		rt_sem_take(test_sem,			/// 获取信号量 
                RT_WAITING_FOREVER); 	/// 等待时间：一直等 
		
		 ModBus_Device.temperature=DS18B20_Get_Temp();
		
		OLED_ShowNum(48,4,i++,5,16);
		
		rt_sem_release(	test_sem );		///释放二值信号量
		
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_4);
		
		rt_thread_mdelay(500);/* 延时500个tick */	
		
	}

}


/**@brief       线程2
* @return       无
*/
static void task2_OLED_entry(void* parameter)
{	

	uint32_t i=0;
	uint8_t buf[5];
	OLED_Init();				/// 初始化OLED 
	OLED_Clear(); 				/// 清除屏幕
	OLED_ShowCHinese(0,0,0);	///
	OLED_ShowCHinese(16,0,1);	///
	OLED_ShowCHinese(32,0,2);	///
	OLED_ShowCHinese(48,0,3);	///
	OLED_ShowCHinese(64,0,4);	///

	OLED_ShowString(0,4,"task1:",16);
	OLED_ShowString(0,6,"task2:",16);
	while(1)
	{
		sprintf((char*)buf,"%d.%d", ModBus_Device.temperature/10,( ModBus_Device.temperature%10));
		
		rt_sem_take(test_sem,			/// 获取信号量 
                RT_WAITING_FOREVER); 	/// 等待时间：一直等 
		
		OLED_ShowString(80,0,buf,16);
		OLED_ShowNum(48,6,i++,5,16);

		rt_sem_release(	test_sem );		/// 释放二值信号量
		
		
		if(850 ==  ModBus_Device.temperature)
		{
			OLED_ShowString(0,2,"Ds18b20Error!",16);
			while(850 ==  ModBus_Device.temperature)
			{
				rt_thread_mdelay(10);
			}
			OLED_ShowString(0,2,"             ",16);
		}
		
		rt_thread_mdelay(500);/* 延时500个tick */
		
	}
}

/**@brief       ModBus协议初始化
* @return       RT_EOK：初始化成功
*/
int Device_Param_Init(void)
{
	Check_Device_Param();
	
	return RT_EOK;
}
INIT_BOARD_EXPORT(Device_Param_Init);
