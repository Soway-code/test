/**
* @file         task.c
* @brief        rt-thread�߳�Ӧ��
* @details      �����̡߳�ִ������
* @author       weijianx
* @date         2020-07-02
* @version      V1.0.0
* @copyright    2020-2030,��������Ϊ�Ƽ���չ���޹�˾
**********************************************************************************
* @par �޸���־:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/07/02  <td>1.0.0    <td>weijianx  <td>������ʼ�汾
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



uint16_t ADBUF[AD_BUF_MAX];                                     //�ɼ�ADֵ��������  10����
float  LiquidHeight = 0;
uint16_t LiquidUnit = 0;

extern uint8_t StartFlag;
extern uint8_t StartCountFlag;
extern BitAction  PulseFlag;


extern UserTypeDef UserPara;
 
uint8_t LFilCnt = 0;
uint8_t time_tick = 10;  //Ĭ���˲���ʽ  Ϊƽ���˲�


extern uint8_t Time_1s_flag;
//extern uint8_t Time_5s_flag;
extern uint16_t Pulse100msCntBuf[3];
extern uint16_t tim_cnt;   //��ʱ1S ʱ���ۼ�

extern uint16_t Current_PositiveTime ;  //��ǰ��ת������
extern uint16_t Current_NegativeTime ;  //��ǰ��ת������
extern uint16_t tim_1min_cnt; //1minʱ���ۼ�



static rt_thread_t dynamic_thread = RT_NULL;//��̬�߳̿��ƿ�ָ��

/****************�̺߳�������******************************/
void led_thread_entry(void *parameter);
//void UART_thread_entry(void *parameter);
void Z_F_Zhuan_thread_entry(void *parameter);
/**********************************************************/
	
	
/*************************************************************************
*                             �߳̽ṹ������(�ʺ��̶߳�̬����)
*************************************************************************
*/
TaskStruct TaskThreads[] = {
			{"ledThread",  led_thread_entry,   RT_NULL,  512,  5, 10},
//			{"UARTThread",  UART_thread_entry,   RT_NULL,  512,  5, 10},
			{"Z_F_Zhuan_Thread", Z_F_Zhuan_thread_entry,  RT_NULL,  512,  2, 10},
			
			/*********************************************************/
			//�ı�ջ�Ĵ�С���ɸı��߳�������Ҳ��ͨ���ı� rtconfig.h �е� RT_MAIN_THREAD_STACK_SIZE �� FINSH_THREAD_STACK_SIZE ��С�����ı��߳�������
			//�û�����̲߳���
			//���磺{�߳�����,�߳���ں���,�߳���ں�������,�߳�ջ��С,�̵߳����ȼ�,�߳�ʱ��Ƭ},
			
			
			
			{"",RT_NULL, RT_NULL,RT_NULL,RT_NULL,RT_NULL}
	
};


/**
 *@brief      ��ʼ������
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
			 
				dynamic_thread = rt_thread_create(TaskThreads[TaskThreadIndex].name,       // �߳����� 
                                 TaskThreads[TaskThreadIndex].entry,                       // �߳���ں��� 
                                 TaskThreads[TaskThreadIndex].parameter,                   // �߳���ں�������
                                 TaskThreads[TaskThreadIndex].stack_size,                  // �߳�ջ��С 
                                 TaskThreads[TaskThreadIndex].priority,                    // �̵߳����ȼ� 
                                 TaskThreads[TaskThreadIndex].tick                         // �߳�ʱ��Ƭ
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
 *@brief      ����ת����ִ��
 *@para       None
 *@retval     None
 */
void Z_F_Zhuan_thread_entry(void *parameter)
{
	
//	uint8_t Z_F_Zhuan;     
//    uint8_t uTemp[4];
    
	while(1)
	{

//		User_Iwdg_Feed();                                                   //ι��
//        
        MBASC_Function();                                                   //MODBUS����
     
/*        
       if(Time_5s_flag)  //10s ʱ�䵽 ���� ����  
       {
         
          
         Time_5s_flag = 0;
                        
         long32Array(UserPara.TotalPulse, uTemp);                               // ����  ��������     ��λ��HZ 
		 FlashWriteDoubleWord(PULSE_TOTAL_BASE, (uint64_t*)uTemp, 4);
//         Eeprom_WriteNBytes(PULSE_TOTAL_BASE, uTemp, 4);
         
         long32Array(UserPara.PositiveTimeBase, uTemp);                         // ����  ��תʱ��   ��λ������
		 FlashWriteDoubleWord(POSITIVE_ROTATE_TIME_BASE, (uint64_t*)uTemp, 4); 
//         Eeprom_WriteNBytes(POSITIVE_ROTATE_TIME_BASE, uTemp, 4);  
         
         long32Array(UserPara.NegativeTimeBase, uTemp);                         // ����  ��תʱ��   ��λ������
		 FlashWriteDoubleWord(NEGATIVE_ROTATE_TIME_BASE, (uint64_t*)uTemp, 4);  
//         Eeprom_WriteNBytes(NEGATIVE_ROTATE_TIME_BASE, uTemp, 4);  
         
         if(UserPara.DirSta != Stall )//��ת���߷�תʱ      20200410 ����
         {
            UserPara.WorkTime = UserPara.PositiveTimeBase + UserPara.NegativeTimeBase + (UserPara.Duration + 30)/60;    //������ʱ�� ��λ����
         }
         else                  //ͣתʱ
         {
            UserPara.WorkTime = UserPara.PositiveTimeBase + UserPara.NegativeTimeBase ;    //������ʱ�� ��λ����
         
         }
         
         
         UserPara.WorkTime = UserPara.WorkTime/6;    // ����  �ۼ�����ʱ��   ��λת��  ����--> 0.1h
         long32Array(UserPara.WorkTime, uTemp);
		 FlashWriteDoubleWord(WORK_TIME_BASE, (uint64_t*)uTemp, 4);   
//         Eeprom_WriteNBytes(WORK_TIME_BASE, uTemp, 4);                         
         
         
         if(UserPara.DirSta==1)// ��ת
         {
             UserPara.Duration = UserPara.PositiveTimeBase - Current_PositiveTime; //  ��ת�������ʱ��    
         }
         else if(UserPara.RotateSta==2)// ��ת
        { 
            UserPara.Duration = UserPara.NegativeTimeBase - Current_NegativeTime;//  ��ת�������ʱ��   
         }
         else
         {
            UserPara.Duration = (tim_1min_cnt + 30) / 60;//  ͣת     ����ͣתʱ��
         }
//         long32Array(UserPara.Duration, uTemp);       // ����  ��ǰ״̬����ʱ��            ��λ 100ms = 0.1s
//         Eeprom_WriteNBytes(DURATION_BASE, uTemp, 4); 
       }
        
       if(Time_1s_flag)    //1s �ж�һ�� ���µ�ǰ״̬
       {     
          Time_1s_flag = 0;
         
          Z_F_Zhuan = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_9);
          if(Z_F_Zhuan == 0) //��ת    //  �͵�ƽ ��ת   �޸�����ת������Э��һ��  20200527        
          {
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
//              Led_Control(0);   //���
          }
          else
          {
			  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
//              Led_Control(1);  //��ת  �̵�
          }
//          UserPara.RotateSpeed  = 0;
//          for(i = 0; i<3;i++ )
//          {
//            UserPara.RotateSpeed +=Pulse100msCntBuf[i];                       
//          }
//          UserPara.RotateSpeed *= 20;  //������ת�ٶ�  1s������*10 =10��* 6 = 1����  ��λ��תÿ��
       
            if(PulseFlag)  //������  ��ת��
            {           
                            
                //Z_F_Zhuan = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_9);
                
                if(Z_F_Zhuan == 0) //��ת
                {
                    if(UserPara.DirSta == Reversal) //��һ��״̬��  ��ת
                    {
                        Current_PositiveTime = UserPara.PositiveTimeBase;
                        UserPara.NegativeTimeBase += (tim_1min_cnt + 30)/60;          //���㷴תʱ��  +30��˼�ǳ����������1����
                        tim_1min_cnt = 0;  //��0��ǰ״̬��ʱ
                        tim_cnt = 0;                    }
                    if(UserPara.RotateSta == STA_STOP)   //��һ��״̬��  ͣת
                    {
                        Current_PositiveTime = UserPara.PositiveTimeBase;
                        tim_1min_cnt = 0;  //��0��ǰ״̬��ʱ
                        tim_cnt = 0;
                    }
                                            
                    UserPara.DirSta = Foreward;   // ==1  //��ת����  ��ת
                             
                    if(!((tim_cnt +1)%60))//60s��  ����һ����תʱ��
                    {
                        UserPara.PositiveTimeBase += 1;        //60s ����һ����תʱ��  
                        tim_1min_cnt = 0;
                    }
                }
                else               //��ת
                {
                    if(UserPara.DirSta == Foreward) //��һ��״̬��  ��ת
                    {
                        Current_NegativeTime = UserPara.NegativeTimeBase;
                        UserPara.PositiveTimeBase += (tim_1min_cnt + 30)/60;          //������תʱ��
                        tim_1min_cnt = 0;  //��0��ǰ״̬��ʱ
                        tim_cnt = 0;
                    }
                    if(UserPara.RotateSta == STA_STOP)   //��һ��״̬��  ͣת
                    {
                        Current_NegativeTime = UserPara.NegativeTimeBase;
                        tim_1min_cnt = 0;  //��0��ǰ״̬��ʱ
                          tim_cnt = 0;
                    }
                                            
                    UserPara.DirSta = Reversal;   // ==0  //��ת����  ��ת
                    
                     if(!((tim_cnt +1)%60))//60s��  ����һ�η�תʱ��
                    {
                        UserPara.NegativeTimeBase += 1;          //60s  ����һ�η�תʱ��  
                        tim_1min_cnt = 0;
                     }                   
                }
                
                UserPara.RotateSta = STA_WORK;                      //��ת״̬   ת����
                
            }
            else     //������  ֹͣ
            {
                    if(UserPara.DirSta == Foreward)                      //��һ��״̬��  ��ת
                    {
                        UserPara.PositiveTimeBase += (tim_1min_cnt + 30)/60;             //������תʱ��  
                        tim_1min_cnt = 0;                                       //��0��ǰ״̬��ʱ
                    }           
                     if(UserPara.DirSta == Reversal)                //��һ��״̬��  ��ת
                    {
                        UserPara.NegativeTimeBase += (tim_1min_cnt + 30)/60;              //���㷴תʱ��
                        tim_1min_cnt = 0;                                      //��0��ǰ״̬��ʱ
                    }

                                  
                UserPara.DirSta = Stall; // ==0  //��ת���� 
                UserPara.RotateSta = STA_STOP;     //��ת״̬   ͣת
                
            }  

       } 
*/	   
	}

}




