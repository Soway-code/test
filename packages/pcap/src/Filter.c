#include "BSP.h"
#include <string.h>

//#include "stm32l0xx_hal_tim.h"
//#include "time.h"
extern void *rt_memcpy(void *dst, const void *src, rt_ubase_t count);

BitAction Add_Sub_Flag = Bit_RESET;
BitAction Calibrate_Flag = Bit_RESET;
extern BitAction Read_Add_Sub_FLag;
uint16_t Add_Sub_Cnt = 0;
TIM_HandleTypeDef TIM3_Handler;      //��ʱ�����
TIM_HandleTypeDef *htim;
TIM_HandleTypeDef TIM2_Handler;      //��ʱ�����
//TIM_HandleTypeDef *htim2;

//extern rt_sem_t filter_sem;
extern struct rt_semaphore filtersem;
//extern struct rt_event nbtempsend_event;

//extern RunVarTypeDef RunVar; 
//rt_sem_t filter_sem;
extern void send_capto_pc(void *p_arg);

void TIM3_IRQHandler(void)
{
	rt_interrupt_enter();
	 if(__HAL_TIM_GET_FLAG(&TIM3_Handler, TIM_FLAG_UPDATE) != RESET)
	 {
		 if(__HAL_TIM_GET_IT_SOURCE(&TIM3_Handler, TIM_IT_UPDATE) !=RESET)
		 { 
			 __HAL_TIM_CLEAR_IT(&TIM3_Handler, TIM_IT_UPDATE);
	        if(PCap_Res_Stau() & 0x100000)                                          //���PCap�Ĳɼ�״̬
	        {
	            RunVar.RawCap = PCap_Res_Value();                                   //��ȡ�ɼ�PCapֵ
	            PCap_MEASURE();                                                     //����PCapģʽ        

						//  rt_kprintf("getRawCap %d \n",RunVar.RawCap);
	        }
	        if(Read_Add_Sub_FLag)       //���ݱ���1.6��(�ն˻��ڶ�ȡһ�����ݲ��ɹ�ʱ1�����ٴζ�ȡ��1.6s��֤�ܹ���ȡ2��)
	        {
	            if(++Add_Sub_Cnt >= 16)                                     
	            {
	             rt_kprintf("���ݱ���1.6�� %d\n",Read_Add_Sub_FLag);
	                Read_Add_Sub_FLag = Bit_RESET;                                  
	                Add_Sub_Flag = Bit_RESET;
	                ProductPara.AddOil = 0;
	                ProductPara.SubOil = 0;
	            }
	        }	
	    }
	//    OSIntExit();
			rt_interrupt_leave();
 }
}
void TIM2_IRQHandler(void)
{
	rt_interrupt_enter();
	 if(__HAL_TIM_GET_FLAG(&TIM2_Handler, TIM_FLAG_UPDATE) != RESET)
	 {
		 if(__HAL_TIM_GET_IT_SOURCE(&TIM2_Handler, TIM_IT_UPDATE) !=RESET)
		 { 
			 __HAL_TIM_CLEAR_IT(&TIM2_Handler, TIM_IT_UPDATE);
       rt_kprintf("TIM2_comin\n");
//			
//	       
	    }
			rt_interrupt_leave();
 }
}

//�ص���������ʱ���жϷ���������
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//    if(htim==(&TIM3_Handler))
//    {rt_kprintf("��ʱ���жϷ�����comin!\n");
//       // LED1=!LED1;        //LED1��ת
//    }
//}

void Timer3_Init(void)
{  
    TIM3_Handler.Instance=TIM3;                          //ͨ�ö�ʱ��3
    TIM3_Handler.Init.Prescaler=4799;                     //��Ƶ
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM3_Handler.Init.Period=999;                        //�Զ�װ��ֵ 999
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//ʱ�ӷ�Ƶ����
    HAL_TIM_Base_Init(&TIM3_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM3_Handler); //ʹ�ܶ�ʱ��3�Ͷ�ʱ��3�����жϣ�TIM_IT_UPDATE    
}
void Timer2_Init(void)
{  
    TIM2_Handler.Instance=TIM2;                          //ͨ�ö�ʱ��3
    TIM2_Handler.Init.Prescaler=4799;                     //��Ƶ
    TIM2_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    //���ϼ�����
    TIM2_Handler.Init.Period=19999;                        //�Զ�װ��ֵ 999
    TIM2_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//ʱ�ӷ�Ƶ����
    HAL_TIM_Base_Init(&TIM2_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM2_Handler); //ʹ�ܶ�ʱ��3�Ͷ�ʱ��3�����жϣ�TIM_IT_UPDATE    
}


//void Pcap_INTN(void)
//{
//    EXTI_InitTypeDef   EXTI_InitStructure;
//    GPIO_InitTypeDef   GPIO_InitStructure;
//    NVIC_InitTypeDef   NVIC_InitStructure;
//    
//    BSP_IntVectSet(BSP_INT_ID_EXTI4_15, EXTI4_15_IRQHandler);
//    
//    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);
//
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
//    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource10);
//
//    EXTI_InitStructure.EXTI_Line = EXTI_Line10;
//    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//    EXTI_Init(&EXTI_InitStructure);
//
//    NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPriority = 3;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
//}

void Capture_TimerInit(void)
{
    Timer3_Init();
}

void Capture_Timer2Init(void)
{
    Timer2_Init();
}


uint16_t CalcuFloaterDACode(uint16_t usRate, FloaterCalibTypeDef *pFcal)
{
    uint16_t i = 0;
    uint16_t head, tail;
    uint16_t u32Temp;

    for(i = 0; i < 8; i++)
    {
        if(usRate <= pFcal->Scale[i])
            break;
    }

    if(i == 0)  
    {
        return pFcal->DACode[0];
    }
    else if(i == 8) 
    {
        head = 6;
        tail = 7;
    }
    else
    {
        head = i - 1;
        tail = i;
    }

    /*
       usRate - pFcal->Scale[head]      pFcal->Scale[tail]  - pFcal->Scale[head]
      ---------------------------- = -------------------------------------------
        x - pFcal->DACode[head]         pFcal->DACode[tail] - pFcal->DACode[head]
       */

    u32Temp = (usRate - pFcal->Scale[head]);
    u32Temp *= (pFcal->DACode[tail] - pFcal->DACode[head]);
    u32Temp /= (pFcal->Scale[tail]  - pFcal->Scale[head]);
    u32Temp += pFcal->DACode[head];

    return((uint16_t)u32Temp);
}

BitAction IsNeetResetPcap(void)
{
    static unsigned char ucRawCapEqualCnt = 0;
    BitAction bErrHappen = Bit_RESET;

    if(RunVar.RawCapBak == 0xffffffff)
    {
        RunVar.RawCapBak = RunVar.RawCap + 1;
    }

    if(RunVar.RawCapBak != RunVar.RawCap)
    {
        ucRawCapEqualCnt = 0;
    }
    else
    {
        ucRawCapEqualCnt += 1;
    }

    RunVar.RawCapBak = RunVar.RawCap;

    if(ucRawCapEqualCnt >= 10)
    {
        ucRawCapEqualCnt = 0;
        bErrHappen = Bit_SET;
    }
    return bErrHappen;
}


#define ADTIMEINTERVAL 100

uint16_t DAOutPutStabilised(uint16_t usRate, uint16_t usDACOutput)
{
    static uint16_t usBaseRate = 0xffff;
    static uint16_t usDABak;
    static uint8_t ucDirBak = 0;     //0��ʾ��ȣ�1��ʾ��С��2��ʾ����
    static uint16_t usLargeThan5PercCnt = 0;
    static uint16_t usLargeThan1PercCnt = 0;

    uint16_t usRateDelta;
    uint8_t ucDir;

    //��һ�����е�ʱ���ȡһ����������
    if (usBaseRate > 101)    //����usRate�Ǹ���ת��������������������1
    {
        usBaseRate = usRate;
        usDABak = usDACOutput;
        ucDirBak = 0;
        usLargeThan5PercCnt = 0;
        usLargeThan1PercCnt = 0;
        
        return usDABak;
    }

    if(usBaseRate == usRate)
    {
        ucDirBak = 0;
        usLargeThan5PercCnt = 0;
        usLargeThan1PercCnt = 0;
        
        return usDABak;
    }
    else if(usBaseRate > usRate)
    {
        ucDir = 1;
        usRateDelta = usBaseRate - usRate;
    }
    else
    {
        ucDir = 2;
        usRateDelta = usRate - usBaseRate;
    }

    //������ı䣬����ı������Ҳ����ı��жϵĻ�׼
    if(ucDir != ucDirBak)
    {
        ucDirBak = ucDir;
        usLargeThan5PercCnt = 0;
        usLargeThan1PercCnt = 0;
        
        return usDABak;
    }
    else
    {
        usLargeThan5PercCnt = (usRateDelta > 5) ? (usLargeThan5PercCnt + 1) : 0;

        usLargeThan1PercCnt = (usRateDelta > 1) ? (usLargeThan1PercCnt + 1) : 0;

        if((usLargeThan1PercCnt > (5000/ADTIMEINTERVAL)) || (usLargeThan5PercCnt > (6000/ADTIMEINTERVAL)))
        {
            usBaseRate = usRate;
            usDABak = usDACOutput;
            //ucDirBak = 0;
            usLargeThan5PercCnt = 0;
            usLargeThan1PercCnt = 0;
            
            return usDACOutput;
        }
        else
        {
            return usDABak;
        }
    }
}




/*
u8 ALGO_BubbleOrderFilter(u32 *pWDataBuf, u8 wLen, u8 wHeadLen, u8 wTailLen, u32 *pAverage)
{
  u8 i;
  u8 j;
  u32 wTemp;
  u32 u32FilterBuf[100];
    
  if (wLen < (wHeadLen + wTailLen))
  {
    return (0);
  }

  for(i = 0; i < wLen; i++)
  {
    u32FilterBuf[i] = *(pWDataBuf + i);
  }
  
  for (i = 0; i < wTailLen; i++)
  {
    for (j = 0; j < wLen - i - 1; j++)
    {
      if(u32FilterBuf[j] > u32FilterBuf[j + 1])
      {
        wTemp = u32FilterBuf[j];
        u32FilterBuf[j] = u32FilterBuf[j + 1];
        u32FilterBuf[j + 1] = wTemp;
      }
    }
  }

  for (i = 0; i < wHeadLen; i++)
  {
    for (j = wLen - wTailLen - 1; j > i; j--)
    {
      if (u32FilterBuf[j - 1] > u32FilterBuf[j])
      {
        wTemp = u32FilterBuf[j - 1];
        u32FilterBuf[j - 1] = u32FilterBuf[j];
        u32FilterBuf[j] = wTemp;
      }
    }
  }
    
  wTemp = 0;
  for(i = wHeadLen; i < wLen - wTailLen; i++)//dsdfsdf
  {
    wTemp += u32FilterBuf[i];
  }
    
  *pAverage = wTemp/(wLen - wHeadLen - wTailLen);
    
  return (1);
}
*/


uint32_t u60sFilter[60];                                                             //60���˲�ר��                
uint32_t u60sFilterBak[60];                                                          //60���˲���������                
EverySecFilTypeDef SecFilStr;

FlagStatus Get_EverySecPcap(void)                                               //���ÿ��ĵ���ֵ
{
    uint8_t i;
    
    RunVar.CapFromPCap = RunVar.RawCap;                                         //��ö�ȡ�ĵ���ֵ
  
    if(RunVar.CapFromPCap > ProductPara.CapMax)                                 //��ȡ�ĵ���ֵ�ڱ궨��������
    {
        RunVar.CapFromPCap = ProductPara.CapMax;
    }
    else if(RunVar.CapFromPCap < ProductPara.CapMin)
    {
        RunVar.CapFromPCap = ProductPara.CapMin;
    }
    
    if(Calibrate_Flag == Bit_SET)            //�·��˱궨������������                       
    {
        SecFilStr.FilterStart = Bit_RESET;      //���˱궨����Ҫ���³�ʼ������ 
        Calibrate_Flag = Bit_RESET;
				rt_kprintf("1st_evercap_filtstart\n");
    }
    
    if(SecFilStr.FilterStart == Bit_RESET)    //��ʼ״̬��������                                   
    {
        SecFilStr.Ms100_Cycle = 0;
        SecFilStr.EverySecCap = 0;
        for(i = 0; i < sizeof(SecFilStr.FilArray)/sizeof(SecFilStr.FilArray[0]); i++)
        {
            SecFilStr.FilArray[i] = RunVar.CapFromPCap;
        }
        for(i = 0; i < sizeof(UserParam.HFil)/sizeof(UserParam.HFil[0]); i++)
        {
            UserParam.HFil[i] = RunVar.CapFromPCap;
        }
        for(i = 0; i < sizeof(UserParam.LFil)/sizeof(UserParam.LFil[0]); i++)
        {
            UserParam.LFil[i] = RunVar.CapFromPCap;
        }
        for(i = 0; i < 60; i++)
        {
            u60sFilter[i] = RunVar.CapFromPCap;
        }
        UserParam.PCap_Filter = RunVar.CapFromPCap;
        SecFilStr.FilterStart = Bit_SET;
				
				rt_kprintf("1st_evercap_filtend %d\n",UserParam.PCap_Filter);
        return RESET;
    }
    SecFilStr.FilArray[SecFilStr.Ms100_Cycle++] = RunVar.CapFromPCap;           //������
    if(SecFilStr.Ms100_Cycle >= 10)                                             //�����ˣ�ȥͷβ���ֵ
    {
        SecFilStr.EverySecCap = GetDelExtremeAndAverage(SecFilStr.FilArray,SecFilStr.Ms100_Cycle,SecFilStr.Ms100_Cycle/3,SecFilStr.Ms100_Cycle/3);
        SecFilStr.Ms100_Cycle = 0;
				
				rt_kprintf("get_SecFilStr.EverySecCap %d\n",SecFilStr.EverySecCap);
        return SET;                                                            //���ÿ��ĵ���ֵ
    }
    return RESET;
}



#include "string.h"
#define FILTER_LEN      3                                                       //������С���˷����ݸ���                                                   
//#define MULTIPLE        20                                                      //�����©������ϵ������
//#define CAL_LEN         FILTER_LEN * MULTIPLE                                   //1�����˲����飬��Ϊ10��
//#define CAL_GRO         CAL_LEN / 6                                             //1���ӷ�Ϊ10�飬ȥ������С��Ϊ��ʼֵ
//#define CAL_CHN         CAL_LEN / CAL_GRO

int xAxis[FILTER_LEN]={0,1,2};                                                  //ÿ��X���ֵ�������ͬ������ȡ�����ͬ��������ΪX��ֵ�Ϳ�����                    
int yAxis[FILTER_LEN];                                                          //ADֵY������

//u32 STA_END[CAL_GRO];                                                           //�Ѽ�©��֮ǰ/������ݲɼ�1���ӣ���10�Σ��ó���ʼ�ͽ���ֵ
//u32 CalSted[CAL_LEN];                                                           //�����©����ʼ�ͽ���ֵ����
//u32 CalStedBak[CAL_LEN];                                                      //�����©����ʼ�ͽ���ֵ���鱸��
//u32 CalStedBakChn[CAL_CHN];
BitAction InitArrayFlag = Bit_RESET;                                            //��ʼ�������־
BitAction Add_Finish_Flag = Bit_RESET;                                          //������ɱ�־        
BitAction Sub_Finish_Flag = Bit_RESET;                                          //©����ɱ�־
BitAction Add_Sub_Start_Flag = Bit_RESET;                                       //��©����ʼ��־                                     
uint8_t UpCnt,DoCnt,WaCnt,TimeCnt,FinishTimeCnt;                                     //Һλ�����½�ʱ�����
uint32_t MinAd,MaxAd,AddAd;                                                          
int OilAddValue,OilSubValue;


//��©���н�״̬
typedef enum
{
    IDLE = 0,
    ADD,
    SUB
}OilTypedef;


//��©����ʼ״̬
typedef enum
{
    STOP = 0,
    STRT,
}Add_SubTypedef;


OilTypedef OilState;
Add_SubTypedef AddState,SubState;
uint8_t LFilCnt;

//ȡ����ֵ������С���˷����õ���λ�ı仯�ʣ��ɴ��жϼ�©�����
void Judge_Add_Sub_Oil(uint32_t SecPCap)
{
    uint8_t i;
    uint16_t ADValue, AddLiqThr, SubLiqThr;
    uint32_t RealSecPCap;
    float Rate;
    int yK;
    //int BlindAreaLen;                                                         //ä������

    if(ProductPara.CompenEn == 1)
    {                                                                           //���ÿ�벹���ĵ���ֵ
        RealSecPCap = (uint32_t)(SecPCap * UserParam.UserFlowK + UserParam.UserFlowB * 100 - 100);
    }
    else
    {
        RealSecPCap = SecPCap;                                                  //�������ĵ���ֵ
    }
                                                                                //����Һλռ�����̵İٷֱ�
    Rate = (RealSecPCap - ProductPara.CapMin) * 1.0f / ProductPara.CapRange;
    if(Rate > 1.0f)
    {
        Rate = 1.0f;
    }
    ADValue = (uint32_t)(Rate * ProductPara.Range);     //�͸�������ΪAD�Ļ�׼ֵ(���̵�λ0.1mm)
	//	rt_kprintf("Judge ADValue %d\n",ADValue);

    if(InitArrayFlag == Bit_RESET)
    {
        InitArrayFlag = Bit_SET;
        for(i = 0; i < FILTER_LEN; i++)
        {
            yAxis[i] = ADValue;
        }
        /*for(i = 0; i < CAL_LEN; i++)
        {
            CalSted[i] = ADValue;
        }*/
    }
 
    //memcpy((u8*)CalSted, (u8*)(CalSted + 1), (CAL_LEN - 1) * 4);
    //*(CalSted + CAL_LEN - 1) = ADValue;      

    rt_memcpy((uint8_t*)yAxis, (uint8_t*)(yAxis+1), (FILTER_LEN-1) * 4);
    *(yAxis+FILTER_LEN-1) = ADValue;          //FIFO����Y��AD����   
    yK = get_slop(xAxis, yAxis, FILTER_LEN); //��С���˷���������ֵ��б�ʣ���ÿ��Һλ���ߵĸ߶�*1000mm�� 
		rt_kprintf("get_slop yk %d\n",yK);
    ProductPara.BottomArea = (ProductPara.BoxPara[0] - 2 * ProductPara.BoxPara[3])\
                                * (ProductPara.BoxPara[1] - 2*ProductPara.BoxPara[3]) / 1000000.0f;//ƽ����
    ProductPara.AddMapHeight = ProductPara.AddLiqCnt * 100.0f / ProductPara.AddLiqTime / ProductPara.BottomArea;//���õ���ֵ��ӳ���ĸ߶�*1000mm
    ProductPara.SubMapHeight = ProductPara.SubLiqCnt * 100.0f / ProductPara.SubLiqTime / ProductPara.BottomArea;
    AddLiqThr = ProductPara.AddLiqCnt;            
    SubLiqThr = ProductPara.SubLiqCnt; 
    
    if(RunVar.AccStatus == 0x02)                                                //ACC����
    {
        if(RunVar.CarSpeed <= 5)                                                //�ж��ٶȣ�С�ڵ���5KM/H  
        {
            AddLiqThr = (uint16_t)(1.5f * ProductPara.AddLiqCnt);                    //����������ֵΪ�趨������ֵ��1.5��
            SubLiqThr = (uint16_t)(1.5f * ProductPara.SubLiqCnt);                    //�����½���ֵΪ�趨������ֵ��1.5��
        }
        else                                                                    //����5KM/H  22��
        {
            AddLiqThr = (uint16_t)(2.2f * ProductPara.AddLiqCnt);                    //����5km/h��˵�������˶��������ֵ
            SubLiqThr = (uint16_t)(2.2f * ProductPara.SubLiqCnt);
        } 
    }
    if(RunVar.CarSpeed > 8)
    {
        AddLiqThr = (uint16_t)(2.2f * ProductPara.AddLiqCnt);    //����5km/h��˵�������˶��������ֵ
        SubLiqThr = (uint16_t)(2.2f * ProductPara.SubLiqCnt);        
    }
         
    if(++TimeCnt <= 20)                                                         //20����
    {
        if((yK > 0) && (yK > (uint32_t)ProductPara.AddMapHeight))        //б�ʴ���0�Ҵ�����ֵб��
        {
            UpCnt++;                                                            //Һλ��������+1
        }
        else if((yK < 0) && (abs(yK) > (uint32_t)ProductPara.SubMapHeight))
        {
            DoCnt++;                                                            //Һλ�½�����+1
        }
        else                                                                    
        {
            WaCnt++;                                                            //��������                
        }
    }
   if(TimeCnt == 20)                                                           //20��һ������
    {
        TimeCnt = 0;
        if(UpCnt >= 8 * (DoCnt + WaCnt))                                        
        {
            if(OilState != ADD)
            {
                OilState = ADD;                                                 //���ڼ���
                if(AddState == STOP)
                {
                    SubState = STOP;
                    AddState = STRT;
                    if(Add_Sub_Start_Flag == Bit_RESET)                 
                    {
                        /*for(i = 0; i < CAL_GRO; i++)
                        {                                                       //�����ݽ����˲�                
                            memcpy((u8*)CalStedBakChn, (u8*)(CalSted + CAL_CHN * i), CAL_CHN * 4);
                            STA_END[i] = GetDelExtremeAndAverage(CalStedBakChn, CAL_CHN, CAL_CHN / 3, CAL_CHN / 3);
                        }
                        MinAd = Get_Min_Max(STA_END, CAL_GRO, 0);               //�����Сֵ��Ϊ�������  */
                        MinAd = RunVar.LiquidHeight;
                        MaxAd = 0;
                        Add_Sub_Start_Flag = Bit_SET;                           //��ʼ��©�ͱ�־��λ
                    }
                }
            }
        }
        else if(DoCnt >= 8 * (UpCnt + WaCnt))        
        {
            if(OilState != SUB)
            {
                OilState = SUB;                                                 //����©��
                if(SubState == STOP)
                {
                    AddState = STOP;
                    SubState = STRT;
                    if(Add_Sub_Start_Flag == Bit_RESET)
                    {
                        /*for(i = 0; i < CAL_GRO; i++)
                        {
                            memcpy((u8*)CalStedBakChn, (u8*)(CalSted + CAL_CHN * i), CAL_CHN * 4);
                            STA_END[i] = GetDelExtremeAndAverage(CalStedBakChn, CAL_CHN, CAL_CHN / 3, CAL_CHN / 3);
                        }
                        MaxAd = Get_Min_Max(STA_END, CAL_GRO, 1);*/
                        MaxAd = RunVar.LiquidHeight;
                        MinAd = 0;
                        Add_Sub_Start_Flag = Bit_SET;
                    }
                }       
            }
        }
        else
        {           
            OilState = IDLE;                                                    //��©�ͽ�������ֻ������
        }
        UpCnt = 0;
        DoCnt = 0;
        WaCnt = 0;                                                              //��0
        if(OilState == IDLE)                                                    //����ģʽ��
        {
            FinishTimeCnt++;
            if((AddState == STRT) && (Add_Finish_Flag == Bit_RESET))            //��⵽����
            {            
                /*emcpy((u8*)CalStedBak, (u8*)CalSted, FILTER_LEN * 10 * 4);
                MaxAd = GetDelExtremeAndAverage(CalStedBak, FILTER_LEN * 10,FILTER_LEN * 8, FILTER_LEN);
                BlindAreaLen = ProductPara.BoxPara[2] * 10 + 200 - ProductPara.Range;
                if(BlindAreaLen <= 0)                                           //ä��С��0����ƽ̨�����������
                {
                    BlindAreaLen = 330;                         
                    //ä���̶�Ϊ33mm
                }       
                OilAddValue = Get_TankOil(MaxAd + BlindAreaLen) - Get_TankOil(MinAd + BlindAreaLen);
                if(OilAddValue > AddLiqThr)
                {
                    ProductPara.AddOil = OilAddValue;                           //����ֵ
                    Add_Sub_Flag = Bit_SET;                                     //��©�ͱ�־
                }*/
                Add_Finish_Flag = Bit_SET;
                AddState = STOP;
            }
            if((SubState == STRT) && (Sub_Finish_Flag == Bit_RESET))            //��⵽©��
            {
                /*memcpy((u8*)CalStedBak, (u8*)CalSted, FILTER_LEN * 10 * 4);
                MinAd = GetDelExtremeAndAverage(CalStedBak, FILTER_LEN * 10, FILTER_LEN, FILTER_LEN * 8);
                BlindAreaLen = ProductPara.BoxPara[2] * 10 + 200 - ProductPara.Range;
                if(BlindAreaLen <= 0)                                           //ä��С��0����ƽ̨�����������
                {
                    BlindAreaLen = 330;                                         //ä���̶�Ϊ33mm
                }   
                OilSubValue = Get_TankOil(MaxAd + BlindAreaLen) - Get_TankOil(MinAd + BlindAreaLen);
                if(OilSubValue > SubLiqThr)
                {
                    ProductPara.SubOil = OilSubValue;                           //����ֵ
                    Add_Sub_Flag = Bit_SET;                                     //��©�ͱ�־
                }*/
                Sub_Finish_Flag = Bit_SET;
                SubState = STOP;              
            }
            //MaxAd = 0;
            //MinAd = 0;
        }
        else
        {
            FinishTimeCnt = 0;
            Add_Finish_Flag = Bit_RESET;
            Sub_Finish_Flag = Bit_RESET;
        }
    } 
    if(FinishTimeCnt >= 6)                    //�м�©��120s��
    {
        FinishTimeCnt = 0;
        if((Add_Finish_Flag == Bit_SET) || (Sub_Finish_Flag == Bit_SET))
        {
            /*for(i = 0; i < CAL_GRO; i++)
            {
                memcpy((u8*)CalStedBakChn, (u8*)(CalSted + CAL_CHN * i), CAL_CHN * 4);
                STA_END[i] = GetDelExtremeAndAverage(CalStedBakChn, CAL_CHN, CAL_CHN / 3, CAL_CHN / 3);
            }*/
            //BlindAreaLen = ProductPara.BoxPara[2] * 10 + 200 - ProductPara.SensorLen;
            /*if(BlindAreaLen <= 0)                                               //ä��С��0����ƽ̨�����������
            {
                BlindAreaLen = 330;                                             //ä���̶�Ϊ33mm
            }*/
            if(Add_Finish_Flag == Bit_SET)
            {
                Add_Finish_Flag = Bit_RESET;
                MaxAd = RunVar.LiquidHeight;                   //�м��ͣ����Һλ����Ϊ�˲�ֵ
                //MaxAd = Get_Min_Max(STA_END, CAL_GRO, 1); 
                OilAddValue = Get_TankOil(MaxAd) - Get_TankOil(MinAd);
                if(OilAddValue > AddLiqThr)
                {
                    ProductPara.AddOil = OilAddValue;                           //����ֵ
                    Add_Sub_Flag = Bit_SET;                                     //��©�ͱ�־
                }
            }
            else if(Sub_Finish_Flag == Bit_SET)
            {
                Sub_Finish_Flag = Bit_RESET;
                //MinAd = Get_Min_Max(STA_END, CAL_GRO, 0);
                MinAd = RunVar.LiquidHeight;                                    
                OilSubValue = Get_TankOil(MaxAd) - Get_TankOil(MinAd);
                if(OilSubValue > SubLiqThr)
                {
                    ProductPara.SubOil = OilSubValue;                           //����ֵ
                    Add_Sub_Flag = Bit_SET;                                     //��©�ͱ�־
                }                    
            }
            MaxAd = 0;
            MinAd = 0;  
            Add_Sub_Start_Flag = Bit_RESET;                                     //��©�Ϳ�ʼ��־����
        }
    }
}





void DA_Handle(uint16_t LiquidAD, float rate)
{
    float smfTemp;
    
    if(ProductPara.LiquitHeightforDAMax > 0.1)
    {
        if(LiquidAD >= ProductPara.LiquitHeightforDAMax)
        {
            smfTemp = 1.0;
        }
        else
        {
            smfTemp = LiquidAD / ProductPara.LiquitHeightforDAMax;
        }
    }
    else
    {
        smfTemp = rate;
    }

    RunVar.DAForFloater = CalcuFloaterDACode((uint16_t)(smfTemp * 100.0), &ProductPara.Floater);
    
    RunVar.DAForFloater = DAOutPutStabilised((uint16_t)(smfTemp * 100.0), RunVar.DAForFloater);
    
    if(Bit_RESET == DAOilDebug.bDADebugEn)
    {
        DA_Write(RunVar.DAForFloater);
    }
    if(Bit_RESET == DAOutDebug.bDADebugEn)
    {
        DA2_Write((uint16_t)(rate * (ProductPara.DAMax - ProductPara.DAMin) + ProductPara.DAMin));
    }    
}


uint8_t timecnt = 0;
uint8_t l_flag = 0;
uint32_t Curr_LiquidHeight = 0;
uint32_t Last_LiquidHeight = 0;

void Get_Filter_PCap(void)
{
    float rate;
    int difference;
//		char buffer[10]={0x00};
    
    if(Get_EverySecPcap())                                                      //�õ�ÿ������
    {    
        if(UserParam.FilterLevel == 1)                                          //ʵʱ�˲�
        {
            UserParam.PCap_Filter = SecFilStr.EverySecCap;                      //�˲�ֵ��Ϊÿ���ֵ
        }
        else if(UserParam.FilterLevel == 2)                                     //ƽ���˲�60s����������
        {
            rt_memcpy((uint8_t*)u60sFilter, (uint8_t*)(u60sFilter + 1), 236);
            *(u60sFilter+59) = SecFilStr.EverySecCap;
            rt_memcpy((uint8_t*)u60sFilterBak, (uint8_t*)u60sFilter, 240);
            UserParam.PCap_Filter = GetDelExtremeAndAverage(u60sFilterBak, 60, 20, 20);          
				}
        else                                                                    //ƽ����ƽ���˲�
        {                                                                       //���˲�ֵ����FIFO
            rt_memcpy((uint8_t*)UserParam.LFil, (uint8_t*)(UserParam.LFil+1), (UserParam.FilterBufMax-1)*4);
            *(UserParam.LFil+UserParam.FilterBufMax-1) = SecFilStr.EverySecCap; //��ǰֵ�����β 
            if(++LFilCnt >= UserParam.FilterBufMax)                             //���˲���������
            {
                LFilCnt = 0;
                rt_memcpy((uint8_t*)UserParam.LFilBak, (uint8_t*)UserParam.LFil, UserParam.FilterBufMax*4);//���˲�ֵ���뱸������
                rt_memcpy((uint8_t*)UserParam.HFil, (uint8_t*)(UserParam.HFil+1), 36);       //���˲�ֵ����FIFO    ��ֵ�����β
                *(UserParam.HFil+9) = GetDelExtremeAndAverage(UserParam.LFilBak,UserParam.FilterBufMax,UserParam.FilterBufMax/3,UserParam.FilterBufMax/3);
                rt_memcpy((uint8_t*)UserParam.HFilBak, (uint8_t*)UserParam.HFil, 40);        //���뱸������
                UserParam.PCap_Filter = GetDelExtremeAndAverage(UserParam.HFilBak,10,3,3);
            }
        }    
        rate = (UserParam.PCap_Filter - ProductPara.CapMin) * 1.0f / ProductPara.CapRange;
//        sprintf(buffer, "%f",rate);
//				rt_kprintf("get_filter_pcap %s %d %d %d\n",buffer,UserParam.PCap_Filter, ProductPara.CapMin,ProductPara.CapRange);
  
				if(rate > 1.0f) rate = 1.0f;
        else if(rate < 0.00001) rate = 0;
        RunVar.LiquidAD = (uint16_t)(rate * 65535);  //Һλ�߶�AD     
        RunVar.LiquidPercent = (uint16_t)(rate * 1000.0f); //�����̵İٷֱ�
        //Curr_LiquidHeight == min 55 max 1151 d ref!  163991                
       // Curr_LiquidHeight = (uint32_t)(rate * ProductPara.Range + ProductPara.BoxPara[2] * 10 - ProductPara.SensorLen + 200);//������ײ���Һλ�߶�    
				Curr_LiquidHeight = rate * ProductPara.SensorLen*10;
        if(!l_flag)                                                             //�ϵ�ִ��һ�Σ����Һλ��ʼֵ
        {
            Last_LiquidHeight = Curr_LiquidHeight;                              //��ʼ��Һλ�߶�
            RunVar.LiquidHeight = Curr_LiquidHeight;
            l_flag = 1;
        }
        difference = Curr_LiquidHeight - Last_LiquidHeight;                     //��ǰ���ϴεĲ�ֵ        

				if(1)//(Add_Sub_Start_Flag)                   //��©��״̬ʱʵ��������
        {                     
            RunVar.LiquidHeight = Curr_LiquidHeight;                            //��ʼ��©��Һλ�߶�Ҫ����Ϊʵʱ�߶�        
            Last_LiquidHeight = Curr_LiquidHeight;
            timecnt = 0;
						
						rt_kprintf("LiquidHeight&diff %d %d \n",RunVar.LiquidHeight,Curr_LiquidHeight);
        }
        else                                                                    //�Ǽ�©��״̬
        {
            if(timecnt++ >= 15)       //15sҺλ�仯һ��
            {
                timecnt = 0;
                if(abs(difference) > 1)                                         //Һλ�仯����0.1mm
                {
                    if(difference > 0)                                          //ֻ�仯0.1mm
                    {
                        RunVar.LiquidHeight += 1;
                    }
                    if(difference < 0)                                          //�½�0.4mm��ԭ���ǻ������������½���
                    {
                        if(difference < -4)
                        {
                            RunVar.LiquidHeight -= 4;
                        }
                        else
                        {
                            RunVar.LiquidHeight += difference;
                        }
                    }
                }        
                Last_LiquidHeight = RunVar.LiquidHeight;
            }
        }       
        RunVar.OilQuantity = Get_TankOil(RunVar.LiquidHeight); //���Ҷ�Ӧ�����������������Ҫ���ڼ�©���ж�ǰ�棬��Ϊ��©���ж���Ҫ�õ����ֵ
       // rt_kprintf("RunVar.OilQuantity %x %x\n",RunVar.LiquidHeight,RunVar.OilQuantity);
				Judge_Add_Sub_Oil(SecFilStr.EverySecCap);                //����Ϊÿ��Pcapֵ����ǰҺλ�߶�ֵ
        DA_Handle(RunVar.LiquidAD, rate);                        //DA���

		}
		
		//rt_sem_release(&filtersem);
}




void App_Filter_Task (void *p_arg)
{
//		rt_err_t result = RT_NULL;
    BitAction bNeetResetPcap = Bit_RESET;
//	rt_uint32_t e;
	rt_mutex_t filtertask = RT_NULL;
    Capture_TimerInit();
  //  DA_Init();
    Last_LiquidHeight = Curr_LiquidHeight;
		
//		filter_sem = rt_sem_create("filtersem",1,RT_IPC_FLAG_FIFO);
//		if(filter_sem == RT_NULL)
//		{
//			rt_kprintf("Failed to create filter_sem semaphore! \n");
//			return ;
//		}
		if (filtertask == RT_NULL)
		{
			/* create AT socket lock */
			filtertask = rt_mutex_create("filtertask", RT_IPC_FLAG_FIFO);
			if (filtertask == RT_NULL)
			{
				rt_kprintf("No memory for filtertask allocation lock!\n");
				return ;
			}
		}

    while(1)
    {
    
		rt_mutex_take(filtertask, RT_WAITING_FOREVER);
		rt_enter_critical();
//#if IWDG_EN > 0
//        IWDG_ReloadCounter();
//#endif  
    //  OSTaskSemPend(0, OS_OPT_PEND_BLOCKING, NULL, &Err);
//			result = rt_sem_take(filter_sem,RT_WAITING_FOREVER);
//			if(result == RT_EOK)
//			{
//					rt_sem_take(&filtersem, RT_WAITING_FOREVER);
//					rt_kprintf("Get_Filter_PCap_comin\n");

//			if (rt_event_recv(&nbtempsend_event,
//								( 16 ),
//								RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
//								RT_WAITING_FOREVER,
//								&e) == RT_EOK)

	        bNeetResetPcap = IsNeetResetPcap();                                 //�Ƿ���Ҫ����PCap
	        
	        if(Bit_SET == bNeetResetPcap)
	        {
	            bNeetResetPcap = Bit_RESET;
	            resetPcap();                                                    //����
	            RunVar_Init();
	            Capture_TimerInit();
							rt_kprintf("Get_Filter_PCap_comin1\n");
	            continue;
	        }
	        Get_Filter_PCap(); 
//			rt_sem_release(filter_sem);
//			rt_event_send(&nbtempsend_event,32);	
        rt_exit_critical();
				rt_mutex_release(filtertask);//frank cl
				rt_thread_mdelay(500);

//	    }
		
   }
}

