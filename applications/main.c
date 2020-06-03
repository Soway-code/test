/*
 * Copyright (c) 2006-2018, Soway Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-6-02      frank liu    first ver
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <Swy_nbapp.h>
#include <para.h>
#include <sensor_dallas_ds18b20.h>
#include <stm32l0xx.h>
#include <algorithm.h>
#include <drv_usart.h>
#include <modbus_asc.h>
#include <stdlib.h>
#include <type.h>

static uint8_t str[17] ={0X3A,0X32,0X31,0X30,0X34,0X32,0X31,0X30,0X30,0X30,0X30,0X30,0X32,0X42,0X38,0X0D,0X0A};// "hello RT-Thread!\r\n";

//static rt_err_t result;
extern void TIM3_IRQHandler(void);
extern void MBASCII_AutoUpLoadFrame(void);
extern void Rd_Dev_Param(void);
extern void PCap_Init(void);
extern void RunVar_Init(void);
extern void Swy_big_endian_cpy(unsigned char * dest, const void *data, int size);
extern void MODBUS_ASCII_HexToAscii(uint8_t cyHexData, uint8_t *pCyAsciiBuf);

extern void upload_sensor_data_thread(void *parameter);
extern void  APP_send_temp_cmd_Task(void *p_arg);
extern void App_Filter_Task (void *p_arg);
extern	void Capture_TimerInit(void);
rt_device_t tcap_serial_dev;
extern RunVarTypeDef RunVar; 
extern long list_thread(void);
//static rt_base_t int_lvl;
rt_mutex_t serial_slock;
extern rt_sem_t send485_cmd_sem;
uint32_t get_ds18b20_temp;
rt_mutex_t mindraytask = RT_NULL;
struct rt_event nbtempsend_event;
extern void  App_Temp_Task(void *p_arg);
extern void send_capto_pc(void *p_arg);

extern void Capture_Timer2Init(void);

 #define SAMPLE_UART_NAME       "uart1"      /* 串口设备名称 */

/* 串口接收消息结构*/
struct rx_msg
{
    rt_device_t dev;
    rt_size_t size;
};
/* 串口设备句柄 */
//static rt_device_t serial;
/* 消息队列控制块 */
static struct rt_messagequeue rx_mq;

/* 接收数据回调函数 */
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    struct rx_msg msg;
    rt_err_t result;
    msg.dev = dev;
    msg.size = size;

    result = rt_mq_send(&rx_mq, &msg, sizeof(msg));
    if ( result == -RT_EFULL)
    {
        /* 消息队列满 */
        rt_kprintf("message queue full！\n");
    }
    return result;
}

static void serial_thread_entry(void *parameter)
{
    struct rx_msg msg;
    rt_err_t result;
    rt_uint32_t rx_length;
    static unsigned char rx_buffer[64],temp_buf[4];
	  rt_uint32_t e;
		
    while (1)
    {
  		if (rt_event_recv(&nbtempsend_event,  //check frank
							( 8 ),
							RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
							RT_WAITING_FOREVER,
							&e) == RT_EOK)
  		{ rt_kprintf("serial_e %d\n",e);
			 	rt_enter_critical();
        rt_memset(&msg, 0, sizeof(msg));
        /* 从消息队列中读取消息*/
        result = rt_mq_recv(&rx_mq, &msg, sizeof(msg), RT_WAITING_FOREVER);
        if (result == RT_EOK)
        {
            /* 从串口读取数据*/
            rx_length = rt_device_read(msg.dev, 0, rx_buffer, msg.size);
            rx_buffer[rx_length] = '\0';
            /* 通过串口设备 serial 输出读取到的消息 */
          //  rt_device_write(serial, 0, rx_buffer, rx_length);
						
						Swy_big_endian_cpy(temp_buf, rx_buffer+12, 3);
						get_ds18b20_temp= hextodec_fun(temp_buf); 
						RunVar.TempInAir = get_ds18b20_temp;
					  rt_kprintf("tem %d\n",RunVar.TempInAir);
		    }
			 rt_exit_critical();
				
			 rt_event_send(&nbtempsend_event,32);	 //16 frank cl
			}
    }
}

 int uart_dma_sample(void)
{
    rt_err_t ret = RT_EOK;//result = RT_EOK;
    char uart_name[RT_NAME_MAX];
    static char msg_pool[128];
    static int arg =0x200;
	  static struct rt_thread serial2_Task;	
	  static char serial2_stack[512];
		
    rt_strncpy(uart_name, SAMPLE_UART_NAME, RT_NAME_MAX);
		
    /* 查找串口设备 */
    tcap_serial_dev = rt_device_find(uart_name);
    if (!tcap_serial_dev)
    {
        rt_kprintf("find %s failed!\n", uart_name);
        return RT_ERROR;
    }

    /* 初始化消息队列 */
    rt_mq_init(&rx_mq, "rx_mq",
               msg_pool,                 /* 存放消息的缓冲区 */
               sizeof(struct rx_msg),    /* 一条消息的最大长度 */
               sizeof(msg_pool),         /* 存放消息的缓冲区大小 */
               RT_IPC_FLAG_FIFO);        /* 如果有多个线程等待，按照先来先得到的方法分配消息 */

    /* 以 DMA 接收及轮询发送方式打开串口设备 */ //RT_DEVICE_FLAG_RDWR
    rt_device_open(tcap_serial_dev, RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_DMA_RX);
		rt_device_control(tcap_serial_dev, 0x03, &arg);
		
    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(tcap_serial_dev, uart_input);
    /* 发送字符串 */
  //  rt_device_write(tcap_serial_dev, 0, str, (sizeof(str) - 1));
		
    /* 创建 serial 线程 */
		rt_thread_init(&serial2_Task,
										"serial_Task",
										serial_thread_entry,
										RT_NULL,
										&serial2_stack[0],
										sizeof(serial2_stack),
										5, 10);
		rt_thread_startup(&serial2_Task);
		
    return ret;
}
int main(void)
{
	 static struct rt_thread upload_task; 
	 static char upload_stack[1024];
	 
//	 static struct rt_thread mindray_Task; 
//	 static char mindray_stack[512];
	 rt_uint32_t e;
//	 static rt_thread_t tid1 = RT_NULL;
	 static rt_thread_t tid2 = RT_NULL;
	 rt_thread_mdelay(30000);

	rt_event_init(&nbtempsend_event, "event", RT_IPC_FLAG_FIFO);
	rt_event_send(&nbtempsend_event,32);
	if (serial_slock == RT_NULL)
	{
		/* create AT socket lock */
		serial_slock = rt_mutex_create("serial_slock", RT_IPC_FLAG_FIFO);
		if (serial_slock == RT_NULL)
		{
			rt_kprintf("No memory for serial_slock allocation lock!\n");
			return -1;
		}
	}
	
	if (mindraytask == RT_NULL)
	{
		/* create AT socket lock */
		mindraytask = rt_mutex_create("mindraytask", RT_IPC_FLAG_FIFO);
		if (mindraytask == RT_NULL)
		{
			rt_kprintf("No memory for mindraytask allocation lock!\n");
			return -1;
		}
	}
	
//	Capture_Timer2Init();
	 rt_thread_init(&upload_task,
									"upload_Task",
									upload_sensor_data_thread,
									RT_NULL,
									&upload_stack[0],
									sizeof(upload_stack),
									15, 10);
	  rt_thread_startup(&upload_task);

		rt_pin_mode(CTRL_485PINa, PIN_MODE_OUTPUT); 
		rt_pin_write(CTRL_485PINa, PIN_LOW);
			
			ReadPara();
			Rd_Dev_Param();
			PCap_Init();
			RunVar_Init();
	//		Capture_TimerInit();
  		 
//			tid1 = rt_thread_create("Create Cap Task",
//															 create_cap_task, 
//															 RT_NULL,
//															 256,
//															 25, 
//															 10);
//					 if (tid1 != RT_NULL)
//						rt_thread_startup(tid1);
				
	//	rt_pm_request(PM_SLEEP_MODE_DEEP);
	
	uart_dma_sample(); //check frank 
	
	tid2 = rt_thread_create("Filter Task",
							App_Filter_Task, 
							RT_NULL,
							512,	//THREAD_STACK_SIZE
							20, 
							10);//THREAD_PRIORITY  THREAD_TIMESLICE
	
		   if (tid2 != RT_NULL)
			rt_thread_startup(tid2);

			 			 
//    rt_thread_init( &mindray_Task,
//                     "mindray Task",
//										 send_capto_pc, 
//										 RT_NULL,
//										 &mindray_stack[0],
//										 sizeof(mindray_stack),
//										 8, 
//										 10);
// 
//				 rt_thread_startup(&mindray_Task);
			 
  while (1)
  {
     // rt_pm_request(PM_SLEEP_MODE_NONE);
     
	   rt_mutex_take(mindraytask, RT_WAITING_FOREVER);
	 if (rt_event_recv(&nbtempsend_event,
						 ( 4 ),
						 RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
						 RT_WAITING_FOREVER,
						 &e) == RT_EOK)
	 	{
		// list_thread();
		// rt_thread_mdelay(1000);
		 	rt_pin_write(CTRL_485PINa, PIN_HIGH); //frank cl
			rt_thread_mdelay(2);
			rt_device_write(tcap_serial_dev,0,str,17);
			rt_pin_write(CTRL_485PINa, PIN_LOW);
			
			rt_thread_mdelay(100);
//			send_capto_pc(NULL);//frank cl
//			rt_thread_mdelay(300);
			
			rt_mutex_release(mindraytask);//frank cl
			rt_event_send(&nbtempsend_event,8);  
		}
   //   rt_pm_release(PM_SLEEP_MODE_NONE);
  }
//  return RT_EOK;
}
