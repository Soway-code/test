/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author         Notes
 * 2019-07-24     WillianChan    the first version
 */

#include <stdlib.h>
#include <rtthread.h>
//#include "sensor.h"
//#include "sensor_dallas_ds18b20.h"
//#include <rtdevice.h>
#include <board.h>
#include <para.h>
#include "algorithm.h"
#include "modbus_asc.h"
#include "type.h"
#include "cap_parameter.h"
#include "drv_usart.h"
#include "algorithm.h"
#include "Swy_nbapp.h"

#define THR(x) (x - 2731) * 10000 
#define TENM_FIL_NUM    10

FlagStatus ReadDataFlag = RESET;
rt_uint32_t ReadDataStartTime;
FlagStatus StaChangeFlag = RESET;
uint32_t TemFilterBuf[TENM_FIL_NUM];
uint32_t TemFilterBufBak[TENM_FIL_NUM];
extern FlagStatus UartRecvFlag;
extern UserTypeDef UserPara;
#define DS18B20_DATA_PIN    GET_PIN(A, 7)
extern uint32_t get_ds18b20_temp;
extern Swy_save_info_t g_swy_data;

extern struct rt_event temp_event;
//extern struct rt_event nbtempsend_event;

#define EVENT_FLAG1 (1 << 1)
#define EVENT_FLAG2 (1 << 2)

extern rt_sem_t dynamic_sem;
//extern rt_sem_t filter_sem;
char uart1_send_comin=0;
//rt_mutex_t serial_slock = RT_NULL;
rt_sem_t send485_cmd_sem;


//                            3A   32    31   30   34   32   31   30   30   30   30   30   32   42   38   0D   0A
//static unsigned char tempstr[] ={0X3A,0X32,0X31,0X30,0X34,0X32,0X31,0X30,0X30,0X30,0X30,0X30,0X32,0X42,0X38,0X0D,0X0A};// "hello RT-Thread!\r\n";
extern void send_capto_pc(void);
void xz_big_endian_cpy(char * dest, const void *data, int size)
{
	unsigned char *src = (unsigned char *)data;

	if (dest == NULL || src == NULL || size < 1)
		return;
	else
	{
		int i = 0;
		for (i = size; i>0; i--)
		{
			rt_memcpy(dest, (src + i - 1), 1);
			dest ++;
		}
	}
}


//static void read_temp_entry(void *parameter)
//{
//    rt_device_t dev = RT_NULL;
//    struct rt_sensor_data sensor_data;
//    rt_size_t res;
//	char ch=0,last_ch = 0,rxbuf[60]={0},read_len=0;
//	rt_bool_t is_full = RT_FALSE;
//    rt_thread_mdelay(13000);
//    dev = rt_device_find(parameter);
//    if (dev == RT_NULL)
//    {
//        rt_kprintf("Can't find device:%s\n", parameter);
//        return;
//    }
////RT_DEVICE_FLAG_RDWR
//    if (rt_device_open(dev, RT_DEVICE_FLAG_INT_RX) != RT_EOK)
//    {
//        rt_kprintf("open device failed!\n");
//        return;
//    }
//    rt_device_control(dev, RT_SENSOR_CTRL_SET_ODR, (void *)100);
//	//rt_kprintf("read_temp_entry!\n");
//
//    while (1)
//		{
//       //at_client_getchar(dev, &ch, RT_WAITING_FOREVER);
//		res = rt_device_read(dev, 0, &ch, 1);//sensor_data
//	 	  if (res != 1)
//	 	  {
//	 		  rt_kprintf("read data failed!size is %d\n", res);
//	 		  rt_device_close(dev);
//	 		  return;
//	 	  }
//        else
//        {
//        if (read_len < 60)
//        {
//            rxbuf[read_len++] = ch;
//           // client->recv_line_len = read_len;
//        }
//        else
//        {
//            is_full = RT_TRUE;
//        }
//        	}
//        /* is newline or URC data */
//        if ((ch == '\n' && last_ch == '\r') )//|| (client->end_sign != 0 && ch == client->end_sign)
//                //|| get_urc_obj(client))
//        {
//            if (is_full)
//            { //rt_kprintf("read_is_full\n");
//                //LOG_E("read line failed. The line data length is out of buffer size!");
//                rt_memset(rxbuf, 0x00, 60);
////                client->recv_line_len = 0;
//                return;//-RT_EFULL;
//            }
//            break;
//        }
//        last_ch = ch;
//    }
//	
//	rt_memset(rxbuf, 0x00, 60);
//	 rt_kprintf("%s\n",rxbuf);
//}
//static int ds18b20_read_temp_sample(void)
//{
//    rt_thread_t ds18b20_thread;
//
//    ds18b20_thread = rt_thread_create("18b20tem",
//                                      read_temp_entry,
//                                      "uart1",  //"temp_ds18b20"
//                                      1024,
//                                      RT_THREAD_PRIORITY_MAX / 2,
//                                      20);
//    if (ds18b20_thread != RT_NULL)
//    {
//        rt_thread_startup(ds18b20_thread);
//    }
//
//    return RT_EOK;
//}
////INIT_APP_EXPORT(ds18b20_read_temp_sample);
//
//static int rt_hw_ds18b20_port(void)
//{
//	rt_device_t dev;//rt_device_find(const char *name)
//    struct rt_sensor_config cfg;
//    dev = rt_device_find("uart1");
//    cfg.intf.user_data = dev;//(void *)DS18B20_DATA_PIN;
//    rt_hw_ds18b20_init("ds18b20", &cfg);
//    
//    return RT_EOK;
//}
////INIT_COMPONENT_EXPORT(rt_hw_ds18b20_port);
static void TemSta_Handle(void)
{
    if(ReadDataFlag == RESET)
    {
        ReadDataStartTime = HAL_GetTick();
    }
    else
    {
        if(HAL_GetTick() - ReadDataStartTime >= 160)
        { 
            ReadDataFlag = RESET;
            StaChangeFlag = RESET;
        }
    }   
}

static void TemData_Handle(void)
{
    static uint32_t TemCountCnt = 0;                                           //必须初始化为0
    static uint32_t ReadTemCnt = 0; 
    static FlagStatus FillBufStatus = RESET;
    static uint8_t temcnt=0;
    int32_t PreTemp;//,testdat;
//		rt_sensor_t sensor;
		//PreTemp = hextodec_fun(get_ds18b20_temp);
	   //xz_big_endian_cpy(&PreTemp,get_ds18b20_temp,4);
		RunVar.TempInAir = get_ds18b20_temp;//ArrToHex(get_ds18b20_temp);//PreTemp/10;
		//sprintf(buffer,"%f",RunVar.TempInAir);
		
		PreTemp = PreTemp-2731;
	  rt_kprintf("check_PreTemp %d %d\n",RunVar.TempInAir,get_ds18b20_temp);
		//testdat=ArrToHex(buffer);
	 // rt_kprintf("testdat %x %d\n",testdat,testdat);

	//Enable_Lpuart1();
    
    if(ReadTemCnt++ <= 2)                                                      //前3s的数据不要（因为会读出个85度）
    {
        return;
    }
        
    if(PreTemp == -27310000)                                                    //初始化18B20不成功,上电会读个85度上来，去掉    
    {
       // ds18b20_init((rt_base_t)sensor->config.intf.user_data);//DS18B20_Init();                                                         //出现故障时复位一下总线
        ReadTemCnt = 0;
        return;
    }
    
    if((PreTemp < -5000000) || (PreTemp > 13000000))                            //温度不在-50~130范围内丢掉
    {
        return;
    }
    if(UartRecvFlag == RESET)                                                   //在接收数据的过程中会产生串口中断
    {
        UserPara.Temp = PreTemp;                                                //影响读数据的准确性,所以在接收数据的过程中读取的数据丢掉
    }
    if(FillBufStatus == RESET)                                                  //开始的十几秒得到稳定数据再打开串口
    {
        TemFilterBuf[temcnt++] = UserPara.Temp;
        if(temcnt >= TENM_FIL_NUM)
        {
            rt_memcpy((uint32_t*)TemFilterBufBak, (uint32_t*)TemFilterBuf, TENM_FIL_NUM * 4);
            UserPara.Temp = GetDelExtremeAndAverage(TemFilterBufBak, TENM_FIL_NUM, TENM_FIL_NUM / 3u, TENM_FIL_NUM / 3u); 
          //  Enable_Lpuart1();
					//	rt_hw_interrupt_enable();
            FillBufStatus = SET;
        }
        else
        {
            return;
        }
    }
    rt_memcpy((uint8_t*)TemFilterBuf, (uint8_t*)TemFilterBuf + 4, (TENM_FIL_NUM - 1) * 4);
    *(TemFilterBuf + (TENM_FIL_NUM - 1)) = UserPara.Temp;                       //得到最新值，滤波
    rt_memcpy((uint8_t*)TemFilterBufBak, (uint8_t*)TemFilterBuf, TENM_FIL_NUM * 4);
    UserPara.Temp = (int)GetDelExtremeAndAverage(TemFilterBufBak, TENM_FIL_NUM, TENM_FIL_NUM / 3u, TENM_FIL_NUM / 3u);    
   // rt_kprintf("get_UserPara.Temp %d\n",UserPara.Temp);
    if(UserPara.Temp >  THR(UserPara.Up_Thr))     //高于最高值
    { rt_kprintf("AlarmSta_comin!!\n");
        if(!(UserPara.AlarmSta & 0x00010000))   //从正常状态跳转到高于高值
        {
            if(TemCountCnt++ >= 3)//UserPara.Du_Thr - 1)     //如果持续超过阀值时间
            {         
                TemCountCnt = 0;
                UserPara.AlarmSta |= 0x00010000;   //报警
                UserPara.AlarmSta &= 0x00010000;
				rt_kprintf("AlarmSta_go!!\n");
            }
        }
        else                                      //持续时间+1s
        {
            TemCountCnt = 0;          //如果在阈值附近抖动，计数值重新开始
            UserPara.Duration += 1;
        }
    }
    else if(UserPara.Temp < THR(UserPara.Do_Thr))                               //低于最低值
    {
        if(!(UserPara.AlarmSta & 0x00000001))                                   //从正常状态跳转到低于低值
        {
            if(TemCountCnt++ >= UserPara.Du_Thr - 1)                            //如果持续超过阀值时间
            {
                TemCountCnt = 0;
                UserPara.AlarmSta |= 0x00000001;                                //报警
                UserPara.AlarmSta &= 0x00000001;
            }
        }
        else                         
        {
            TemCountCnt = 0;             //如果在阈值附近抖动，计数值重新开始
            UserPara.Duration += 1;      //持续时间+1s
        }
    }
    else                               //在阈值范围内
    {
        if(UserPara.AlarmSta != 0)               //从报警状态切换到正常状态
        {
            UserPara.Duration += 1;                 //先要继续+1s
            if(TemCountCnt++ >= 10)//UserPara.Du_Thr - 1)  //如果持续超过阀值时间
            {
                TemCountCnt = 0;
                UserPara.AlarmSta = 0;
                UserPara.Duration = 0;
            }
        }
        else
        {
            TemCountCnt = 0;          //如果在阈值附近抖动，计数值重新开始
            UserPara.AlarmSta = 0;
            UserPara.Duration = 0;
        }
		rt_kprintf("UserPara.AlarmSta %d \n",UserPara.AlarmSta);
    }    
}

void  App_Temp_Task(void *p_arg)
{
//	rt_err_t result = RT_NULL;
	rt_uint32_t Alarm_Pre_Sta,Alarm_Cur_Sta;
	rt_uint32_t StartTime;
//	rt_uint32_t e;

	while (1)
	{
//		result = rt_sem_take(temp_sem,RT_WAITING_FOREVER);
//		if(result != RT_EOK)
//		{
//			return;
//		}
//		result = rt_sem_take(dynamic_sem,RT_WAITING_FOREVER);
//		if(result != RT_EOK)
//		 {
//			 return;
//		 }
//if (rt_event_recv(&nbtempsend_event,
//					( 16 ),
//					RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
//					RT_WAITING_FOREVER,
//					&e) == RT_EOK)

//		if (rt_event_recv(&temp_event,
//											(EVENT_FLAG1 | EVENT_FLAG2),
//											RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,
//											RT_WAITING_FOREVER,
//											&e) == RT_EOK)
//		{		   
			 TemSta_Handle(); 
				Alarm_Pre_Sta = UserPara.AlarmSta;		//获得上次的报警状态
				MBASC_Function(); 
			 if(HAL_GetTick() - StartTime >= 100)
			 {
					TemData_Handle();
					Alarm_Cur_Sta = UserPara.AlarmSta;		//获得本次报警状态
					if(Alarm_Cur_Sta != Alarm_Pre_Sta)		//两次报警状态不相同即发生状态变化
					{
							StaChangeFlag = SET;
					}
					StartTime = HAL_GetTick();						
			 }
			 if(HAL_GetTick() - StartTime >= 1000)
			 {
						//UART1_SendData				
			 } 
			 
			 rt_kprintf("temp_comin\n");
//		}
//		rt_sem_release(dynamic_sem);
//		rt_event_send(&nbtempsend_event,32);  //frank cl

		rt_thread_mdelay(3000);
	}
}
void  APP_send_temp_cmd_Task(void *p_arg)
{
//		rt_err_t result = -1;
//		 rt_base_t int_lvl;
//    if (serial_slock == RT_NULL)
//    {
//        /* create AT socket lock */
//        serial_slock = rt_mutex_create("serial_slock", RT_IPC_FLAG_FIFO);
//        if (serial_slock == RT_NULL)
//        {
//            rt_kprintf("No memory for serial_slock allocation lock!\n");
//            return ;
//        }
//    }
//		rt_sem_create("send485_sem", 1,RT_IPC_FLAG_FIFO);
//  rt_kprintf("APP_send_temp_cmd_Task!\n");

	while(1)
	{
	
//			dynamic_sem = rt_sem_create("dsem",1,RT_IPC_FLAG_FIFO);
//			if(dynamic_sem != RT_NULL)
//			{
			//	rt_kprintf("Failed to create dynamic semaphore! \n");
				//return ;
			//}
			
//		result = rt_sem_take(send485_cmd_sem,RT_WAITING_FOREVER);
//		if (result == RT_EOK)
//		{

//	result = rt_sem_take(filter_sem,RT_WAITING_FOREVER);
//	if(result == RT_EOK)
//	{
//	uart1_send_comin =1;

//	 rt_mutex_take(serial_slock, RT_WAITING_FOREVER);
//	rt_mutex_release(serial_slock);

	//  int_lvl = rt_hw_interrupt_disable();
//			UART1_SendData(tempstr, 17);
//		  rt_event_send(&temp_event,4);
//			rt_event_send(&nbtempsend_event,4);
		//	rt_thread_mdelay(200);
		
	  send_capto_pc();

		//	rt_thread_mdelay(1000);
		//	uart1_send_comin =0;
//			 rt_sem_release(filter_sem);
	//	rt_mutex_release(serial_slock);
//		}
//	 rt_sem_release(dynamic_sem);
//	rt_hw_interrupt_enable(int_lvl);

	 rt_thread_mdelay(2000);//g_swy_data.upload_time
	//	rt_kprintf("APP_send_temp_cmd_Task\n");
	}

}


