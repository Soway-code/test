/**@file        task.c
* @brief        ����ģ��
* @details      ����������ִ������
* @author       �����
* @date         2020-08-14
* @version      V1.0.0
* @copyright    2020-2030,��������Ϊ�Ƽ���չ���޹�˾
**********************************************************************************
* @par �޸���־:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/08/01  <td>1.0.0    <td>�����    <td>������ʼ�汾
* </table>
*
**********************************************************************************
*/


#include "task.h"
#include "stdio.h"
#include "flash.h"
#include "usart.h"

static struct rt_thread task1_thread;			 	//�����߳̿��ƿ� 
static rt_uint8_t rt_task1_thread_stack[1024];		//�����߳�ջ

static struct rt_thread task2_thread;				//�����߳̿��ƿ� 
static rt_uint8_t rt_task2_thread_stack[1024];		//�����߳�ջ


//��������
static void task1_thread_entry(void* parameter);
static void task2_thread_entry(void* parameter);


/**@brief       ����ĳ�ʼ��
* @return       ����ִ�н��1
*/
uint8_t task_init(void) 
{
	HAL_UART_Receive_DMA(&huart1, UsartType.RX_pData, RX_LEN);  
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE); 
	FlashPageErase(FLASH_BASE_ADDRESS);
	rt_thread_init(&task1_thread, 						//�߳̿��ƿ� 
					"task1", 							//�߳����� 
					task1_thread_entry, 				// �߳���ں��� 
					RT_NULL, 							// �߳���ں������� 
					&rt_task1_thread_stack[0],			// �߳�ջ��ʼ��ַ 
					sizeof(rt_task1_thread_stack), 		// �߳�ջ��С 
					3, 									// �̵߳����ȼ� 
					20); 								// �߳�ʱ��Ƭ 
	rt_thread_startup(&task1_thread); 					// �����̣߳��������� 
	
	rt_thread_init(&task2_thread, 						// �߳̿��ƿ� 
					"task2", 							// �߳����� 
					task2_thread_entry, 				// �߳���ں��� 
					RT_NULL, 							// �߳���ں������� 
					&rt_task2_thread_stack[0],			// �߳�ջ��ʼ��ַ 
					sizeof(rt_task2_thread_stack), 		// �߳�ջ��С 
					3, 									// �̵߳����ȼ� 
					20); 								// �߳�ʱ��Ƭ 
	rt_thread_startup(&task2_thread); 					// �����̣߳��������� 
	
	return 1;
}

/**@brief       �߳�1
* @return       ��
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
		rt_thread_mdelay(5);/* ��ʱ500��tick */		
	}
}

/**@brief       �߳�2
* @return       ��
*/
static void task2_thread_entry(void* parameter)
{	
	while(1)
	{
		printf("task2\n");
		rt_thread_mdelay(500);/* ��ʱ500��tick */	
	}
}


