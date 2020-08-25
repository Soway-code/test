/**@file        task.c
* @brief        ����ģ��
* @details      ����������ִ������
* @author       �����
* @date         2020-08-25
* @version      V1.0.0
* @copyright    2020-2030,��������Ϊ�Ƽ���չ���޹�˾
**********************************************************************************
* @par �޸���־:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/08/25  <td>1.0.0    <td>�����    <td>������ʼ�汾
* </table>
*
**********************************************************************************
*/

#include "task.h"
#include "DS18B20.h"
#include "DS18B20_1.h"
#include "user_mb_app.h"
#include <rtthread.h>

extern USHORT usSRegHoldBuf[S_REG_HOLDING_NREGS];
uint16_t temp=0;
uint16_t temp1=0;

/**@brief       �߳�1
* @return       ��
*/
static void task1(void *parameter)
{
	DS18B20_Init();	
	DS18B20_Init1();
	uint16_t tem = 0;
	uint16_t tem1 = 0;
 	while(1)
	{
		temp = DS18B20_Get_Temp();
		tem =(temp/10);
		tem<<=8;
		tem|=(temp%10);
		usSRegHoldBuf[0] = tem;
		
		temp1 = DS18B20_Get_Temp1();
		tem1 =(temp1/10);
		tem1<<=8;
		tem1|=(temp1%10);
		usSRegHoldBuf[1] = tem1;
		
		rt_kprintf("tem:%x\n",tem);
		rt_kprintf("tem1:%x\n",tem1);
		rt_thread_mdelay(500);
	}
}

/**@brief       �߳�2
* @return       ��
*/
static void task2(void *parameter)
{
	while(1)
	{
		oled_clearBuffer();
		oled(0,16,temp,temp1);
		rt_thread_mdelay(500);
	}
}

/**@brief       ����ĳ�ʼ������
* @param[in]    ��
* @param[out]   ��
* @return       ��������ʼ�����
* - RT_EOK(�ɹ�)
* - RT_ERROR(ʧ��)
*/
uint8_t task_init(void)
{
	 rt_thread_t tid1 = RT_NULL, tid2 = RT_NULL;
	tid1 = rt_thread_create("task1", task1, RT_NULL, 512, 7, 10);
    if (tid1 != RT_NULL)
    {
        rt_thread_startup(tid1);
    }
    else
    {
        return RT_ERROR;
    }

    tid2 = rt_thread_create("task2", task2, RT_NULL, 512,7, 10);
    if (tid2 != RT_NULL)
    {
        rt_thread_startup(tid2);
    }
    else
    {
         return RT_ERROR;
    }

	return RT_EOK;
	
}

