/*
 * File      : Swy_nbapp.c
 * Copyright Statement:
 * The software may not be copied and the information
 * contained herein may not be used or disclosed except with the written
 * permission of Soway Inc. (C) 2020
 * COPYRIGHT (C) 2020 - 2028, Soway Development Team
 *
 * 28 longguang Street, first Floor, SZ China.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-03-13     frank       first version
 * 2020-03-26     frank       add semaphore
 */

#include <rtthread.h>
#include <at_device_bc28.h>
#include <Swy_nbapp.h>
#include <at_socket.h>
#include <string.h>
#include <stm32l0xx_hal.h> 
#include <type.h>
#include <algorithm.h>
#include <drv_usart.h>


//#define THREAD_PRIORITY         25
//#define THREAD_STACK_SIZE       512
//#define THREAD_TIMESLICE        5

//static rt_thread_t tid1 = RT_NULL;
//static rt_thread_t tid2 = RT_NULL;
//struct rt_event nbupload_event;
#define EVENT_FLAG1 (1 << 1)
#define EVENT_FLAG2 (1 << 2)

struct at_socket *sock = RT_NULL;
struct sockaddr_in server_adr;
rt_sem_t dynamic_sem;
/* 邮箱控制块 */
//static struct rt_mailbox mb;
/* 用于放邮件的内存池 */
//static char mb_pool[64];
//static char mb_str[] = "already";

Swy_tcp_info_t _tcp_conn;
Swy_tcp_info_t _udp_conn;
Swy_mqtt_info_t _mq_conn;
Swy_save_info_t g_swy_data;

rt_int8_t SywAppTcpConnectToServer(const char* host, rt_uint16_t port);
extern void change_upload_inter_time(int timeout);
extern RunVarTypeDef RunVar;                            
extern void MODBUS_ASCII_HexToAscii(uint8_t cyHexData, uint8_t *pCyAsciiBuf);
extern struct rt_event nbtempsend_event;
extern void  APP_send_temp_cmd_Task(void *p_arg);
extern void send_cmd_exec_reset(void);
extern void judge_recv_event_ack(int statue);

#define mqaccount  "866971030521988"
#define mqpasword  "866971030521988"
#define mqsecret   "e938e6daf27427554a01022333b8c5c8"
rt_mutex_t upload_slock = RT_NULL;

int SwyAppReconnectServer(void)
{
	char    res;//*hostname = NULL;
//  if (_udp_conn.state == AT_SOCKET_CONNECT)  //frank cl
	//at_closesocket(server_adr.sock);

	res =  SywAppTcpConnectToServer(Swyurl,Swyport);;
	if(res==0)
	{
		rt_kprintf("re_connect_ser success\n");
	} 
	//SwyAppSetConnectStartTime();
	return res;
}
void Swy_big_endian_cpy(unsigned char * dest, const void *data, int size)
{
	unsigned char *src = (unsigned char *)data;

	if (dest == NULL || src == NULL || size < 1)
		return;
#ifndef WIN32
	if (size < 2)// || _is_big_endian == 1
	{
		memcpy(dest, src, size);
	}
	else
#endif
	{
		int i = 0;
		for (i=0; i<size; i++)
		{
			memcpy(dest, (src + i ), 1);
			dest ++;
		}
	}
}
/******  send to ser   ****/
int Swy_nb_send(const       unsigned char *sendbuf,int n)
{
//		    int ret=0;
//			  static int count=0;
			
				int  temperature = 0;
				int cap_LiquidHeight=0;
				int Swysendlen=0;
				uint8_t end = 0x1a;
				uint8_t enc_data[24] = {0x00};
				uint8_t *p=enc_data;
				uint8_t enc_ascdata[24] = {0x00};
			 // _tcp_conn.send_size = Swysendlen;
				rt_err_t result = RT_NULL;
		 //	rt_memset(_tcp_conn.send_data, 0, sizeof(_tcp_conn.send_data));
		 // p = _tcp_conn.send_data;
				
				cap_LiquidHeight = RunVar.LiquidHeight/10;
				temperature = (int)RunVar.TempInAir/10;
				rt_kprintf("Swy_nb_send %d %d\n",cap_LiquidHeight,temperature);
				Swy_big_endian_cpy(p, &cap_LiquidHeight, 4); 
				p +=4;
				Swysendlen +=4;
				Swy_big_endian_cpy(p, &temperature, 4);
				p +=4;
				Swysendlen +=4;
        for (int i=0;i<Swysendlen;i++)		
		    {
		      MODBUS_ASCII_HexToAscii(enc_data[i],enc_ascdata+i*2);//hexbuf[i]
        }
				rt_memcpy(&enc_ascdata[2*Swysendlen],&end,1);
				Swysendlen = 2*Swysendlen+1;
//				_tcp_conn.send_size = Swysendlen;
//				rt_sem_release(dynamic_sem);
//				rt_pm_release(PM_SLEEP_MODE_NONE);
#if defined(Swy_tcp) || defined(Swy_mqtt)
	result = at_send(server_adr.sock,enc_ascdata,Swysendlen, 0);  //frank cl
#else
	result = at_sendto(server_adr.sock, enc_ascdata, Swysendlen, 0,
						(struct sockaddr *)&server_adr, sizeof(struct sockaddr));
#endif
	if (result <0)
		{
		#ifdef Swy_mqtt
		 _mq_conn.mqtt_open=0;
	   _mq_conn.mqtt_conn=0;
	   _mq_conn.mqtt_sub=0;
		#endif
		// SwyAppReconnectServer();
		return -1;
		}
	else

		{
//		  ret = ctrl_bc26_device_ops("sleep");
//			if (ret < 0)
//			 ctrl_bc26_device_ops("sleep");
		}
	rt_kprintf("Swy_nb_send_result %d\n",result);
	return result ;
}
void Swy_recv_data_handle(const char *data, int len)
{
	//unsigned char com, need_reply=0;
//	unsigned short com_len;
//	int cout = 0;
//	unsigned char token[4] = {0x00,0x00,0x00,0x00};
unsigned char	tempbut[12]= {0};

	char *p=NULL;

#if (Swy_mqtt == 1)
	if(strstr(data,"+QMTOPEN: 0,0"))
	{
	 _mq_conn.mqtt_open = 1;
	}
	else if(strstr(data,"+QMTCONN: 0,0,0"))
	{
	  _mq_conn.mqtt_conn = 1;
	}
	else if(strstr(data,"+QMTSUB: 0,1,0,2"))
	{
	 _mq_conn.mqtt_sub = 1;
	 _mq_conn.state = AT_SOCKET_CONNECT;
	 rt_kprintf("_mqtt_conn.state %d\n",_mq_conn.state);
	}
	if (_mq_conn.mqtt_sub == 1)
	 {
			if(strstr(data,"+QMTPUB: 0,0,0"))
			{  
			   rt_kprintf("QMTPUB success\n");
				 
				 judge_recv_event_ack(1);
			}
//			else
//			{
//			
//			rt_kprintf("QMTPUB fail\n");
//			  // judge_recv_event_ack(0);
//			}
  }
#elif (Swy_tcp ==1 || Swy_udp ==1)
	if(strstr(data,"+QIOPEN: 0,0"))
	{
	 if (Swy_tcp ==1 )
		_tcp_conn.state = AT_SOCKET_CONNECT;
	 else if(Swy_udp ==1 )
	  _udp_conn.state = AT_SOCKET_CONNECT;
	  rt_kprintf("_tcp_conn.state %d\n",_tcp_conn.state);
	}
#endif
	
	if (rt_strstr(data,"set_time"))//(strncmp(data, "set_time", strlen("set_time")) == 0)//
	{ 
	   p = strstr(data, "#");
		 p +=1;
		// data +=54;
		 Swy_big_endian_cpy(tempbut, p, 5);
		
		 g_swy_data.upload_time = atoi((char*)tempbut);
		 if (g_swy_data.upload_time > 5000)
		 rt_kprintf("set upload_time success!%d\n",g_swy_data.upload_time);		
    p=NULL;
	} 
	p = rt_strstr( data,"AT+");	
	if (rt_strncmp( p,"AT+QPOWD=0",strlen("AT+QPOWD=0"))==0)
	{ 
		 rt_kprintf("AT+QPOWD=0 comin1!\n");
		
//		 ctrl_bc26_device_ops("AT+QPOWD=0");
		 p=NULL;
	} 
//	Swy_big_endian_cpy((unsigned char *)&com_len, data+3, 2);// test frank
//	com = *(data +0);
//	if (com == 0x7E && len > 3)
//	{
//		com = *(data +12); 
//		rt_kprintf("com:%x", com);
//		/*第三字节为主信令*/
//		if (com == 0x81)
//		{    
//		    data+=33;
//			Swy_big_endian_cpy(token, data, 4);
//		
//	   }
//			rt_kprintf("token:%x,%x,%x,%x", token[0],token[1],token[2],token[3]);		
//	
//	}
}
void reset_module(void)
{
	rt_pin_write(CTRL_485PINa, PIN_HIGH); //frank cl
	rt_thread_mdelay(2);
	rt_device_write(tcap_serial_dev,0,"AT+NRB",6);
	rt_pin_write(CTRL_485PINa, PIN_LOW);
}
rt_int8_t SywAppTcpConnectToServer(const char* host, rt_uint16_t port)
{
//	  struct hostent *Swyhost;
	//struct sockaddr_in server_addr;
//	  Swyhost = gethostbyname(host);
#if (Swy_tcp == 1 || Swy_mqtt == 1)
		server_adr.sock = at_socket(AF_INET, SOCK_STREAM, 0); 
#elif(Swy_udp == 1)
		server_adr.sock = at_socket(AF_INET, SOCK_DGRAM, 0);
#endif
		if(server_adr.sock == -1)
    {
        
        rt_kprintf("Socket error\n");
				reset_module();
				rt_thread_mdelay(5000);
        return -3;
    }
			
		rt_kprintf("creat_Socket %d\n",server_adr.sock);

    /* init pre connent of ser addr */
    server_adr.sin_family = AF_INET;
    server_adr.sin_port = htons(port);
   // server_addr.sin_addr = *((struct in_addr *)host->h_addr);
		server_adr.sin_addr.s_addr = inet_addr(host);
    rt_memset(&(server_adr.sin_zero), 0, sizeof(server_adr.sin_zero));
		if(server_adr.sock >= 0)
		{
    #if ((Swy_tcp == 1) || (Swy_mqtt == 1)|| (Swy_udp == 1))
		
			if (at_connect(server_adr.sock, (struct sockaddr *)&server_adr, sizeof(struct sockaddr)) == -1)
	    {
	        /* connect fail */
//         rt_kprintf("Connect fail!\n");
//				 ctrl_bc26_device_ops("reset");//fix confail frank
//				 rt_kprintf("reset-NB-MODULE1\n");
//				 HAL_Delay(5000);
					rt_kprintf("at_connect..fail\n");

				if(0== SwyAppReconnectServer())
					{
					 rt_kprintf("reconnect..ok\n");
					 return 0;
					}
				 else
				 	 return -3;
	      // at_closesocket(server_adr.sock);
	       
	    }
	  #endif
		}
   else
		{
		  at_closesocket(server_adr.sock);
      return -2;
  	}
		rt_kprintf("connect_ser ok\n");
		return 0;
}

void upload_sensor_data_thread(void *parameter)
{
	  rt_int32_t send_len=0,ret=-1;
//		rt_err_t result = -1;
		rt_uint32_t e;
		static rt_int32_t sockstate = 0;
 	if (upload_slock == RT_NULL)
 	{
 		/* create AT socket lock */
 		upload_slock = rt_mutex_create("upload_slock", RT_IPC_FLAG_FIFO);
 		if (upload_slock == RT_NULL)
 		{
 			rt_kprintf("No memory for upload_slock allocation lock!\n");
 			return ;
 		}
 	}
//		dynamic_sem = rt_sem_create("dsem",1,RT_IPC_FLAG_FIFO);
//		if(dynamic_sem == RT_NULL)
//		{
//			rt_kprintf("Failed to create dynamic semaphore! \n");
//			return ;
//		}
		rt_kprintf("upload_sensor_data_thread! \n");

		if (g_swy_data.upload_time < 5000 )
		{
	  	g_swy_data.upload_time = 20000;// default 10s
	  	
		}

		if(Swy_udp == 1)
		{
			SywAppTcpConnectToServer(Swyurl,Swyport);
		}
		while(1)
		{  
		
	    rt_mutex_take(upload_slock, RT_WAITING_FOREVER); //frank cl
		//rt_enter_critical();
		//rt_pm_request(PM_SLEEP_MODE_NONE);
		//result = rt_sem_take(dynamic_sem,RT_WAITING_FOREVER);
		 if (rt_event_recv(&nbtempsend_event,  //temp close frank cl
							( 32 ) ,
							RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
							RT_WAITING_FOREVER,
							&e) == RT_EOK)
  	 {
	     if (Swy_tcp == 1)
	    	{
	        sockstate = _tcp_conn.state ;
	    	}
	     else if (Swy_mqtt == 1)
	      { 
	        sockstate = _mq_conn.state;
	  	  }
			  else if (Swy_udp == 1)
		  	{
	        sockstate = _udp_conn.state ;
		  	}
				
		    if (sockstate != AT_SOCKET_CONNECT && Swy_udp !=1)
		 		{
				 
				   ret = SywAppTcpConnectToServer(Swyurl,Swyport);
					 if (ret == 0)
					 	_tcp_conn.state = AT_SOCKET_CONNECT;
					 else 
					 	{ 
					  	rt_kprintf("upload_connect_ser fail!! \n");					
//					 	  send_cmd_exec_reset();
//						 rt_thread_mdelay(5000);
					 	}
					
		    }
				if((ret == 0) ||(sockstate == AT_SOCKET_CONNECT) || (Swy_udp == 1))
				{

		 			 send_len=Swy_nb_send(_tcp_conn.send_data,_tcp_conn.send_size);
					 if(send_len > 0)
					 {  
					    rt_kprintf("send ok\n");
		//			  rt_event_send(&nbtempsend_event,4);  //frank cl
					 } 
			     else 
					 {
					  rt_kprintf("send fail\n");
					//Swy_nb_send(_tcp_conn.send_data,_tcp_conn.send_size);
	//				rt_kprintf("reset_NB\n");
	//				ctrl_bc26_device_ops("reset");
	//				send_cmd_exec_reset();
	//				rt_thread_mdelay(5000);
				   	at_closesocket(server_adr.sock);  //frank cl
						SwyAppReconnectServer();
	 					rt_thread_mdelay(5000);
	      	 }
				}
			 else
			  {
				  ret = SwyAppReconnectServer();
					if (ret !=0)
				  ret = SywAppTcpConnectToServer(Swyurl,Swyport);
					rt_kprintf("SwyAppReconnectServer %d\n",ret);
			  }
//	 	 rt_pm_release(PM_SLEEP_MODE_NONE);			 
     }
	 
	 rt_event_send(&nbtempsend_event,4);	//frank cl
   rt_mutex_release(upload_slock);//frank cl
	  // rt_sem_release(dynamic_sem);
	   rt_thread_mdelay(g_swy_data.upload_time);
   	}
	}
void get_sensor_data_thread(void *parameter)
{
//		float  temperature = 0;
//		int cap_LiquidHeight=0;
//		int count = 0,Swysendlen=0;
//		unsigned char *p = _tcp_conn.send_data;
//		rt_err_t result = RT_NULL;
//	  unsigned int sens_interval_time=0;
		
		//rt_pm_request(PM_SLEEP_MODE_DEEP);
//		while(1)
//		{
		
//		  rt_pm_request(PM_SLEEP_MODE_NONE);
//			result = rt_sem_take(dynamic_sem,RT_WAITING_FOREVER);
//			if(result != RT_EOK)
//			{
//				return;
//			}
		/* initializes aht10, registered device driver */
		/*	dev = aht10_init(i2c_bus_name);
			if(dev == RT_NULL)
			{
					rt_kprintf(" The sensor initializes failure");
					return 0;
			}
   	*/
//			  send_to_pc_set();
    #if 0
			rt_memset(_tcp_conn.send_data, 0, sizeof(_tcp_conn.send_data));
     // p = _tcp_conn.send_data;
			/* read humidity */
			cap_LiquidHeight = RunVar.LiquidHeight/10;//aht10_read_humidity(dev);
			//rt_kprintf("humidity	 : %d.%d %%\n", (int)humidity, (int)(humidity * 10) % 10); /* former is integer and behind is decimal */

			/* read temperature */
			temperature = (float)RunVar.TempInAir/10;//aht10_read_temperature(dev);
			// %d.%d \n", (int)temperature, (int)(temperature * 10) % 10); /* former is integer and behind is decimal */
      Swy_big_endian_cpy(p, &cap_LiquidHeight, 4); 
			p +=4;
			Swysendlen +=4;
      Swy_big_endian_cpy(p, &temperature, 4);
			p +=1;
			Swysendlen +=4;
			*p = 0x1a;
			Swysendlen +=1;
      _tcp_conn.send_size = Swysendlen;//xz_encode_send_data(_udp_conn.send_data);
			rt_kprintf("sensor_comin\n");
			#endif
		  //rt_mb_send(&mb, (rt_uint32_t)&mb_str);
		 // sens_interval_time = g_swy_data.upload_time - 500;
//			rt_sem_release(dynamic_sem);
//			rt_pm_release(PM_SLEEP_MODE_NONE);
//			rt_thread_mdelay(sens_interval_time);
//		}
	}

int thread_sample(void)
{   
//    rt_err_t result;
//		dynamic_sem = rt_sem_create("dsem",1,RT_IPC_FLAG_FIFO);
//		if(dynamic_sem == RT_NULL)
//		{
//			rt_kprintf("Failed to create dynamic semaphore! \n");
//			return 1;
//		}
    /* 初始化一个 mailbox */
//    result = rt_mb_init(&mb,
//                        "mbt",                      /* 名称是 mbt */
//                        &mb_pool[0],                /* 邮箱用到的内存池是 mb_pool */
//                        sizeof(mb_pool) / 4,        /* 邮箱中的邮件数目，因为一封邮件占 4 字节 */
//                        RT_IPC_FLAG_FIFO);          /* 采用 FIFO 方式进行线程等待 */
//    if (result != RT_EOK)
//    {
//        rt_kprintf("init mailbox failed.\n");
//        return -1;
//    }

//		tid1 = rt_thread_create("sensor",
//															 get_sensor_data_thread, RT_NULL,
//															 1024,
//															 26, 10);
//		
//			 if (tid1 != RT_NULL)
//					 rt_thread_startup(tid1);
			 
//    tid2 = rt_thread_create("upload",
//                            upload_sensor_data_thread, RT_NULL,
//                            1024,   //THREAD_STACK_SIZE
//                            25, 30);//THREAD_PRIORITY  THREAD_TIMESLICE
//
//    if (tid2 != RT_NULL)
//        rt_thread_startup(tid2);
		
    return 0;
}

