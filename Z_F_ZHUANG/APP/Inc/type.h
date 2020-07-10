#ifndef _TYPE_H
#define _TYPE_H
#include "stm32g0xx_hal.h"

typedef enum
{ 
  Bit_RESET = 0,
  Bit_SET = 1
}BitAction;


typedef enum
{
    STA_STOP = 1,   //停止状态
    STA_WORK = 2,       //运行
}RotateStaTypeDef;   //旋转状态


typedef enum
{
    Stall    = 0,      //停止状态
    Foreward = 1,       //正转状态
    Reversal = 2,       //反转状态
}DirectionState_TypeDef;                 //转动方向


typedef enum
{
    STA_NORMAL = 0,
    STA_TIMEOUT,
}StandbyTypeDef;

typedef struct 
{
    uint8_t SlaveAddr;                                                          //从机地址
    uint8_t Baudrate;		                                                 //波特率
    uint8_t Parity;             			                         //奇偶校验位

    uint8_t ImpDataFlag;                                                        //重要数据标志
    RotateStaTypeDef RotateSta;                                                         //旋转状态    
    DirectionState_TypeDef DirSta;                                                      //转动方向
    
    uint32_t RotateSpeed;                                                       //速度
    
    uint32_t PositiveTimeBase;                                                      //正转时长基值
    uint32_t NegativeTimeBase;                                                      //反转时长基值
    
    uint32_t WorkTime;                                                          //工作时长  0x06-0x07  累计运行时间
    
    uint32_t TotalPulse;                                                        //累计脉冲数量
      
    uint32_t Duration;                                                          //当前状态持续时长      0x0A-0x0B   旋转方向持续时间 
    
   // DirectionState_TypeDef RotateFlag;                                                       //转动标志  
    //uint16_t StaVaryDuration;                                                   //状态变换持续时长 
   // uint32_t StandbyTimeBase;                                                   //总怠速时长基值
    //uint32_t StopTimeBase;                                                      //停机时长基值     
    //uint32_t StandbyTime;                                                       //怠速时长
    //uint32_t StopTime; 
    //停机时长
    
     uint8_t FilterBufMax;                                                //滤波缓存最大值
     uint16_t FilterCycle;                                                 //滤波周期
}UserTypeDef;



/*
typedef struct 
{
    uint8_t SlaveAddr;                                                          //从机地址
    uint8_t Baudrate;		                                                 //波特率
    uint8_t Parity;             			                         //奇偶校验位

    uint8_t Product_type;                                                    //输出类型（00-电压型 默认  01电流型）    
    uint8_t Vout_type;                                                        //电压型输出类别 （ 00:0.5-4.5V默认    01:1-5V     02:0-10V    ）
    uint8_t HeightUnit;                                                      //液位高度单位
   
    uint16_t SensorRange;                                                      //传感器量程
    uint16_t LiquidAdMin;                                                       //液位标定   空值（输出电压为0.5V时的AD值）
    uint16_t LiquidAdMax;                                                       //液位标定  满值（输出电压为4.5V时的AD值） 
    uint8_t FilterLevel;                                                      //滤波等级
    uint8_t No_defined2;                                                         //未定义2
     
    uint16_t AdAvgVol;
    
    uint16_t AD_Value_Filter;
  
    uint16_t LiquidAdVal;                                               //液位AD值
    uint16_t LiquidHeight;                                              //液位高度 
    
    uint8_t FilterBufMax;                                                //滤波缓存最大值
    uint16_t FilterCycle;                                                 //滤波周期
    
    uint32_t HFil[10];                                                                 //高阶滤波数组
    uint32_t HFilBak[10];                                                              //高阶滤波数组备份
    uint32_t LFil[96];                                                                 //低阶滤波数组
    uint32_t LFilBak[96];                                                              //低阶滤波数组备份     
   
}UserTypeDef;
*/

/*
typedef struct 
{
    uint16_t CurCnt;                                                            //当前计数值
    uint16_t PreCnt;                                                            //上次的计数值
    uint16_t OneSecCnt;                                                         //每秒的计数值
}TIM2_ParaDef;
*/


#endif





