/**
* @file         task.c
* @brief        rt-thread线程应用
* @details      创建线程、执行任务
* @author       weijianx
* @date         2020-07-02
* @version      V1.0.0
* @copyright    2020-2030,深圳市信为科技发展有限公司
**********************************************************************************
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/07/02  <td>1.0.0    <td>weijianx  <td>创建初始版本
* </table>
*
**********************************************************************************
*/
#include "task.h"
#include "config.h"
#include "string.h"
#include "type.h"
#include "modbus_asc.h"
#include "common.h"
#include "para.h"
#include "inflash.h"



uint16_t ADBUF[AD_BUF_MAX];                                     //采集AD值缓冲数组  10个数
float  LiquidHeight = 0;
uint16_t LiquidUnit = 0;

extern uint8_t StartFlag;
extern uint8_t StartCountFlag;
extern BitAction  PulseFlag;


extern UserTypeDef UserPara;
 
uint8_t LFilCnt = 0;
uint8_t time_tick = 10;  //默认滤波方式  为平稳滤波


extern uint8_t Time_1s_flag;
//extern uint8_t Time_5s_flag;
extern uint16_t Pulse100msCntBuf[3];
extern uint16_t tim_cnt;   //定时1S 时间累加

extern uint16_t Current_PositiveTime ;  //当前正转脉冲数
extern uint16_t Current_NegativeTime ;  //当前反转脉冲数
extern uint16_t tim_1min_cnt; //1min时间累加



static rt_thread_t dynamic_thread = RT_NULL;//动态线程控制块指针

/****************线程函数声明******************************/
void led_thread_entry(void *parameter);
//void UART_thread_entry(void *parameter);
void Z_F_Zhuan_thread_entry(void *parameter);
/**********************************************************/
	
	
/*************************************************************************
*                             线程结构体数组(适合线程动态创建)
*************************************************************************
*/
TaskStruct TaskThreads[] = {
			{"ledThread",  led_thread_entry,   RT_NULL,  512,  5, 10},
//			{"UARTThread",  UART_thread_entry,   RT_NULL,  512,  5, 10},
			{"Z_F_Zhuan_Thread", Z_F_Zhuan_thread_entry,  RT_NULL,  512,  2, 10},
			
			/*********************************************************/
			//改变栈的大小，可改变线程数量；也可通过改变 rtconfig.h 中的 RT_MAIN_THREAD_STACK_SIZE 或 FINSH_THREAD_STACK_SIZE 大小，来改变线程数量，
			//用户添加线程参数
			//例如：{线程名字,线程入口函数,线程入口函数参数,线程栈大小,线程的优先级,线程时间片},
			
			
			
			{"",RT_NULL, RT_NULL,RT_NULL,RT_NULL,RT_NULL}
	
};


/**
 *@brief      初始化任务
 *@para       None
 *@retval     None
 */
void task_Init(void)
{
	uint8_t TaskThreadIndex = 0;

	 while(1)
	 {
		 if(strcmp(TaskThreads[TaskThreadIndex].name,"") != 0)
		 {
			 
				dynamic_thread = rt_thread_create(TaskThreads[TaskThreadIndex].name,       // 线程名字 
                                 TaskThreads[TaskThreadIndex].entry,                       // 线程入口函数 
                                 TaskThreads[TaskThreadIndex].parameter,                   // 线程入口函数参数
                                 TaskThreads[TaskThreadIndex].stack_size,                  // 线程栈大小 
                                 TaskThreads[TaskThreadIndex].priority,                    // 线程的优先级 
                                 TaskThreads[TaskThreadIndex].tick                         // 线程时间片
                   	            );
				if(dynamic_thread != RT_NULL)
				{
					rt_thread_startup(dynamic_thread);
				}
			  TaskThreadIndex ++;
		 }
		 else
			 break;
		 
	 }
}


void led_thread_entry(void *parameter)
{
//	uint8_t ch[] = "2424\r\n";
	while (1)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_4);
//		UART_DMA_Send(ch, sizeof(ch));
		rt_thread_mdelay(1000);
//		if(Time_2s_flag)
//		{
//			Time_2s_flag = 0;
//			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_4);
//		}

	}

}


/**
 *@brief      正反转任务执行
 *@para       None
 *@retval     None
 */
void Z_F_Zhuan_thread_entry(void *parameter)
{
	
//	uint8_t Z_F_Zhuan;     
//    uint8_t uTemp[4];
    
	while(1)
	{

//		User_Iwdg_Feed();                                                   //喂狗
//        
        MBASC_Function();                                                   //MODBUS处理
     
/*        
       if(Time_5s_flag)  //10s 时间到 更新 数据  
       {
         
          
         Time_5s_flag = 0;
                        
         long32Array(UserPara.TotalPulse, uTemp);                               // 更新  总脉冲数     单位：HZ 
		 FlashWriteDoubleWord(PULSE_TOTAL_BASE, (uint64_t*)uTemp, 4);
//         Eeprom_WriteNBytes(PULSE_TOTAL_BASE, uTemp, 4);
         
         long32Array(UserPara.PositiveTimeBase, uTemp);                         // 更新  正转时间   单位：分钟
		 FlashWriteDoubleWord(POSITIVE_ROTATE_TIME_BASE, (uint64_t*)uTemp, 4); 
//         Eeprom_WriteNBytes(POSITIVE_ROTATE_TIME_BASE, uTemp, 4);  
         
         long32Array(UserPara.NegativeTimeBase, uTemp);                         // 更新  反转时间   单位：分钟
		 FlashWriteDoubleWord(NEGATIVE_ROTATE_TIME_BASE, (uint64_t*)uTemp, 4);  
//         Eeprom_WriteNBytes(NEGATIVE_ROTATE_TIME_BASE, uTemp, 4);  
         
         if(UserPara.DirSta != Stall )//正转或者反转时      20200410 增加
         {
            UserPara.WorkTime = UserPara.PositiveTimeBase + UserPara.NegativeTimeBase + (UserPara.Duration + 30)/60;    //计算总时间 单位分钟
         }
         else                  //停转时
         {
            UserPara.WorkTime = UserPara.PositiveTimeBase + UserPara.NegativeTimeBase ;    //计算总时间 单位分钟
         
         }
         
         
         UserPara.WorkTime = UserPara.WorkTime/6;    // 更新  累计运行时间   单位转换  分钟--> 0.1h
         long32Array(UserPara.WorkTime, uTemp);
		 FlashWriteDoubleWord(WORK_TIME_BASE, (uint64_t*)uTemp, 4);   
//         Eeprom_WriteNBytes(WORK_TIME_BASE, uTemp, 4);                         
         
         
         if(UserPara.DirSta==1)// 正转
         {
             UserPara.Duration = UserPara.PositiveTimeBase - Current_PositiveTime; //  旋转方向持续时间    
         }
         else if(UserPara.RotateSta==2)// 反转
        { 
            UserPara.Duration = UserPara.NegativeTimeBase - Current_NegativeTime;//  旋转方向持续时间   
         }
         else
         {
            UserPara.Duration = (tim_1min_cnt + 30) / 60;//  停转     不计停转时间
         }
//         long32Array(UserPara.Duration, uTemp);       // 更新  当前状态运行时间            单位 100ms = 0.1s
//         Eeprom_WriteNBytes(DURATION_BASE, uTemp, 4); 
       }
        
       if(Time_1s_flag)    //1s 判断一次 更新当前状态
       {     
          Time_1s_flag = 0;
         
          Z_F_Zhuan = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_9);
          if(Z_F_Zhuan == 0) //正转    //  低电平 正转   修改正反转方向，与协议一致  20200527        
          {
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
//              Led_Control(0);   //红灯
          }
          else
          {
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
//              Led_Control(1);  //反转  绿灯
          }
//          UserPara.RotateSpeed  = 0;
//          for(i = 0; i<3;i++ )
//          {
//            UserPara.RotateSpeed +=Pulse100msCntBuf[i];                       
//          }
//          UserPara.RotateSpeed *= 20;  //计算旋转速度  1s脉冲数*10 =10秒* 6 = 1分钟  单位：转每分
       
            if(PulseFlag)  //有脉冲  在转动
            {           
                            
                //Z_F_Zhuan = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_9);
                
                if(Z_F_Zhuan == 0) //正转
                {
                    if(UserPara.DirSta == Reversal) //上一次状态是  反转
                    {
                        Current_PositiveTime = UserPara.PositiveTimeBase;
                        UserPara.NegativeTimeBase += (tim_1min_cnt + 30)/60;          //计算反转时间  +30意思是超过半分钟算1分钟
                        tim_1min_cnt = 0;  //清0当前状态计时
                        tim_cnt = 0;                    }
                    if(UserPara.RotateSta == STA_STOP)   //上一次状态是  停转
                    {
                        Current_PositiveTime = UserPara.PositiveTimeBase;
                        tim_1min_cnt = 0;  //清0当前状态计时
                        tim_cnt = 0;
                    }
                                            
                    UserPara.DirSta = Foreward;   // ==1  //旋转方向  正转
                             
                    if(!((tim_cnt +1)%60))//60s到  更新一次正转时间
                    {
                        UserPara.PositiveTimeBase += 1;        //60s 更新一次正转时间  
                        tim_1min_cnt = 0;
                    }
                }
                else               //反转
                {
                    if(UserPara.DirSta == Foreward) //上一次状态是  正转
                    {
                        Current_NegativeTime = UserPara.NegativeTimeBase;
                        UserPara.PositiveTimeBase += (tim_1min_cnt + 30)/60;          //计算正转时间
                        tim_1min_cnt = 0;  //清0当前状态计时
                        tim_cnt = 0;
                    }
                    if(UserPara.RotateSta == STA_STOP)   //上一次状态是  停转
                    {
                        Current_NegativeTime = UserPara.NegativeTimeBase;
                        tim_1min_cnt = 0;  //清0当前状态计时
                          tim_cnt = 0;
                    }
                                            
                    UserPara.DirSta = Reversal;   // ==0  //旋转方向  反转
                    
                     if(!((tim_cnt +1)%60))//60s到  更新一次反转时间
                    {
                        UserPara.NegativeTimeBase += 1;          //60s  更新一次反转时间  
                        tim_1min_cnt = 0;
                     }                   
                }
                
                UserPara.RotateSta = STA_WORK;                      //旋转状态   转动中
                
            }
            else     //无脉冲  停止
            {
                    if(UserPara.DirSta == Foreward)                      //上一次状态是  正转
                    {
                        UserPara.PositiveTimeBase += (tim_1min_cnt + 30)/60;             //计算正转时间  
                        tim_1min_cnt = 0;                                       //清0当前状态计时
                    }           
                     if(UserPara.DirSta == Reversal)                //上一次状态是  反转
                    {
                        UserPara.NegativeTimeBase += (tim_1min_cnt + 30)/60;              //计算反转时间
                        tim_1min_cnt = 0;                                      //清0当前状态计时
                    }

                                  
                UserPara.DirSta = Stall; // ==0  //旋转方向 
                UserPara.RotateSta = STA_STOP;     //旋转状态   停转
                
            }  

       } 
*/	   
	}

}




