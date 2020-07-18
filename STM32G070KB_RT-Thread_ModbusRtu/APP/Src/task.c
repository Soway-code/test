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
* @brief OS Version: RT-Thread 4.0.2
* @brief para modify log:
* <table>
* <tr><th>Date	<th>Version 
* <tr><td>2020/07/08 <td>1.0 <td>Loodao <td> buidling new version
* </table>
*
***********************************************************************
*/

#include "task.h"

///�����߳̿��ƿ�ָ�� 
static rt_thread_t task1_18B20 = RT_NULL;
static rt_thread_t task2_OLED = RT_NULL;


static rt_sem_t test_sem = RT_NULL;				/// �����ź������ƿ�

uint32_t temperature;							//���18B20�¶�

ModBusBaseParam_TypeDef     ModBusBaseParam;    //ModBus����Ļ��������ṹ
ModBus_Device_Param         ModBus_Device;  	//ModBus�����豸�Ĳ����ṹ��


/**************************************************************************
*                             ��������
*************************************************************************
*/
static void task1_18B20_entry(void* parameter);
static void task2_OLED_entry(void* parameter);


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
	test_sem = 
	rt_sem_create("test_sem",	///�ź������� 
                     1,     				///�ź�����ʼֵ��Ĭ����һ���ź���
                     RT_IPC_FLAG_FIFO); 	///�ź���ģʽ FIFO(0x00)
	
	///�����߳�
	task1_18B20 =                          	///�߳̿��ƿ�ָ�� 
    rt_thread_create( "ds18b20",            ///�߳����� 
                      task1_18B20_entry,   	///�߳���ں��� 
                      RT_NULL,            	///�߳���ں�������
                      512,                 	///�߳�ջ��С
                      3,                   	///�̵߳����ȼ� 
                      20);                 	///�߳�ʱ��Ƭ 
                   

   if (task1_18B20 != RT_NULL)
        rt_thread_startup(task1_18B20);		///�����̣߳��������� 
    else
        return 0;
    
	
    task2_OLED =                           ///�߳̿��ƿ�ָ�� 
    rt_thread_create( "OLED",              ///�߳�����  
                      task2_OLED_entry,    ///�߳���ں���  
                      RT_NULL,             ///�߳���ں������� 
                      512,                 ///�߳�ջ��С 
                      4,                   ///�̵߳����ȼ�  
                      20);                 ///�߳�ʱ��Ƭ  
                   
 
   if (task2_OLED != RT_NULL)
        rt_thread_startup(task2_OLED);		 ///�����̣߳��������� 
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
	uint32_t i=0;
	while(1)
	{
		rt_sem_take(test_sem,			/// ��ȡ�ź��� 
                RT_WAITING_FOREVER); 	/// �ȴ�ʱ�䣺һֱ�� 
		
		 ModBus_Device.temperature=DS18B20_Get_Temp();
		
		OLED_ShowNum(48,4,i++,5,16);
		
		rt_sem_release(	test_sem );		///�ͷŶ�ֵ�ź���
		
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_4);
		
		rt_thread_mdelay(500);/* ��ʱ500��tick */	
		
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

	uint32_t i=0;
	uint8_t buf[5];
	DS18B20_Init();	
	OLED_Init();				/// ��ʼ��OLED 
	OLED_Clear(); 				/// �����Ļ
	OLED_ShowCHinese(0,0,0);	///
	OLED_ShowCHinese(16,0,1);	///
	OLED_ShowCHinese(32,0,2);	///
	OLED_ShowCHinese(48,0,3);	///
	OLED_ShowCHinese(64,0,4);	///
	OLED_ShowCHinese(112,0,5);	///
	OLED_ShowString(0,4,"task1:",16);
	OLED_ShowString(0,6,"task2:",16);
	while(1)
	{
		sprintf((char*)buf,"%d.%d", ModBus_Device.temperature/10,( ModBus_Device.temperature%10));
		
		rt_sem_take(test_sem,			/// ��ȡ�ź��� 
                RT_WAITING_FOREVER); 	/// �ȴ�ʱ�䣺һֱ�� 
		
		OLED_ShowString(80,0,buf,16);
		OLED_ShowNum(48,6,i++,5,16);
		
		rt_sem_release(	test_sem );		/// �ͷŶ�ֵ�ź���
		
		
		if(850 ==  ModBus_Device.temperature)
		{
			OLED_ShowString(0,2,"Ds18b20Error!",16);
			while(850 ==  ModBus_Device.temperature)
			{
				rt_thread_mdelay(10);
			}
			OLED_ShowString(0,2,"             ",16);
		}
		
		rt_thread_mdelay(500);/* ��ʱ500��tick */
		
	}
}


int Device_Param_Init(void)
{
	Check_Device_Param();
	
	return RT_EOK;
}
INIT_BOARD_EXPORT(Device_Param_Init);
