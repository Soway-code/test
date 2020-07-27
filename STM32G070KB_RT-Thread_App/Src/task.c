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
/// �����߳̿��ƿ�ָ�� 
static rt_thread_t task1_18B20 = RT_NULL;
static rt_thread_t task2_OLED = RT_NULL;
static rt_thread_t task3_usart = RT_NULL;

/// ��������
static void task1_18B20_entry(void* parameter);
static void task2_OLED_entry(void* parameter);
static void task3_usart_entry(void* parameter);
uint32_t temperature; 

/// �����ź������ƿ�
static rt_sem_t test_sem = RT_NULL;


/**
 * ����         : uint8_t task_init(void)	
 * ��������     : 2020-07-17
 * ����         : 
 * ����         : ��ʼ�� �����ź���,�߳�
 * �������     : ��
 * �������     : ��
 * ���ؽ��     : 1 �����ɹ�  0 ����ʧ��
 * ע���˵��   : ��
 * �޸�����     : �� 
 */
uint8_t task_init(void)
{
	///����һ���ź���
	test_sem = rt_sem_create("test_sem",	///�ź������� 
                     1,     				///�ź�����ʼֵ��Ĭ����һ���ź���
                     RT_IPC_FLAG_FIFO); 	///�ź���ģʽ FIFO(0x00)
	
	task1_18B20 =                          ///�߳̿��ƿ�ָ�� 
    rt_thread_create( "task1_18B20",       /// �߳����� 
                       task1_18B20_entry,  /// �߳���ں��� 
					   RT_NULL,            /// �߳���ں������� 
                       512,                /// �߳�ջ��С 
                       4,                  /// �̵߳����ȼ� 
                       20);                /// �߳�ʱ��Ƭ 
	 /* �����̣߳��������� */
	if (task1_18B20 != RT_NULL)
        rt_thread_startup(task1_18B20);
	else
       return 0;
    
	
    task2_OLED =                          	/// �߳̿��ƿ�ָ�� 
    rt_thread_create( "task2_OLED",         /// �߳����� 
                       task2_OLED_entry,  	/// �߳���ں��� 
                       RT_NULL,            	/// �߳���ں������� 
                       512,                	/// �߳�ջ��С 
                       4,                   /// �̵߳����ȼ� 
                       20);                	/// �߳�ʱ��Ƭ 
                   
    ///�����̣߳��������� 
	if (task2_OLED != RT_NULL)
        rt_thread_startup(task2_OLED);
    else
        return 0; 
	
	task3_usart =                          	/// �߳̿��ƿ�ָ�� 
    rt_thread_create( "task3_usart",        /// �߳����� 
                       task3_usart_entry,  	/// �߳���ں��� 
                       RT_NULL,            	/// �߳���ں������� 
                       512,                	/// �߳�ջ��С 
                       4,                   /// �̵߳����ȼ� 
                       20);                	/// �߳�ʱ��Ƭ 
                   
    ///�����̣߳��������� 
	if (task3_usart != RT_NULL)
        rt_thread_startup(task3_usart);
    else
        return 0; 
	
	return 1; 

}


/**
 * ����         : task1_18B20_entry(void* parameter)
 * ��������     : 
 * ����         : 
 * ����         : �߳�1
 * �������     : ��
 * �������     : ��
 * ���ؽ��     : ��
 * ע���˵��   : ��
 * �޸�����     : �� 
 */
static void task1_18B20_entry(void* parameter)
{
	
	while(1)
	{
		rt_sem_take(test_sem,			/// ��ȡ�ź��� 
                RT_WAITING_FOREVER); 	/// �ȴ�ʱ�䣺һֱ�� 
		
		temperature=DS18B20_Get_Temp();	
		
		rt_sem_release(	test_sem );		///�ͷŶ�ֵ�ź���
		
		printf("%d\n",temperature);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
		rt_thread_delay(500);
		
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
		rt_thread_delay(500);
	}
}

/**
 * ����         : task2_OLED_entry(void* parameter)
 * ��������     : 
 * ����         : 
 * ����         : �߳�2
 * �������     : ��
 * �������     : ��
 * ���ؽ��     : ��
 * ע���˵��   : ��
 * �޸�����     : �� 
 */
static void task2_OLED_entry(void* parameter)
{

	uint8_t buf[5];
	OLED_Init();				/// ��ʼ��OLED 
	OLED_Clear(); 				/// �����Ļ
	OLED_ShowCHinese(0,0,0);	///
	OLED_ShowCHinese(16,0,1);	///
	OLED_ShowCHinese(32,0,2);	///
	OLED_ShowCHinese(48,0,3);	///
	OLED_ShowCHinese(64,0,4);	///
	OLED_ShowCHinese(112,0,5);	///
	while(1)
	{
		sprintf((char*)buf,"%d.%d",temperature/10,(temperature%10));
		
		rt_sem_take(test_sem,			/// ��ȡ�ź��� 
                RT_WAITING_FOREVER); 	/// �ȴ�ʱ�䣺һֱ�� 
		
		OLED_ShowString(80,0,buf,16);
		
		rt_sem_release(	test_sem );		/// �ͷŶ�ֵ�ź���
		
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
 * ����         : task3_usart_entry(void* parameter)
 * ��������     : 
 * ����         : 
 * ����         : �߳�3
 * �������     : ��
 * �������     : ��
 * ���ؽ��     : ��
 * ע���˵��   : ��
 * �޸�����     : �� 
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
			
			if(0 == strncmp((char*)UART1_RXBuff,(char*)buf,15))		//����ָ��һ��
				HAL_NVIC_SystemReset();								///��λ

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

