#ifndef __TYPE_H
#define __TYPE_H

#include "stm32l0xx.h"	
#include "filter.h"

//#include "Sys_cfg.h"
//#include "os.h"

#define ECHO_ENABLE 	        0                                               //冻结
#define ECHO_DISABLE 	        1                                               //解冻

#define FLASH_WR_ENABLE         1                                               //Flash写使能
#define FLASH_WR_DISABLE        0                                               //Flash写禁止

typedef struct 
{
  uint8_t SlaveAddr;                                                                 //从机地址
  uint8_t BaudRate;                                                                  //波特率
  uint8_t WordLength;                                                                //数据位
  uint8_t StopBits;                                                                  //停止位
  uint8_t Parity;                                                                    //校验位
  uint8_t Echo;                                                                      //是否冻结
}UartCommTypeDef;

typedef struct
{
  uint16_t DACode[8];                                                                //DA码
  uint16_t Scale[8];                                                                 //百分度
}FloaterCalibTypeDef;

typedef struct
{
  uint32_t CapMin;                                                                   //电容零点
  uint32_t CapMax;                                                                   //电容满度
  uint32_t CapRange;                                                                 //电容范围       
  uint16_t Range;                                                                    //量程
  uint16_t SensorLen;                                                                 //传感器长度
  float DAMin;                                                                  //DA零点
  float DAMax;                                                                  //DA满度
  float Def_Range;                                                              //出厂量程
  float Def_CapMin;                                                             //出厂零点
  float Def_CapMax;                                                             //出厂满度
  
  float Temp_K;                                                                 //温度1系数K
  float Temp_B;                                                                 //温度1系数B
  float Temp2_K;                                                                //温度2系数K
  float Temp2_B;                                                                //温度2系数B
  float LiquitHeightforDAMax;                                                   //液位最大DA值
  float BottomArea;                                                             //底面积(单位：平方米) 
  float AddMapHeight;                                                           //加油阈值反映出来的高度（单位：mm）
  float SubMapHeight;                                                           //漏油阈值反映出来的高度（单位：mm）
  uint8_t AutoUpLoadTime;                                                             //自动上传周期
  uint8_t CompenEn;                                                                   //补偿使能
  //u8 OutputUnit;
  uint8_t bFlashWEn;                                                                 //写Flash使能
  uint8_t LiquidCmpTc;                                                               //液位比较时间间隔 2 3 4 5
  uint8_t FuelType;                                                                  //燃料类型
  uint8_t BoxShape;                                                                  //油箱形状
  uint16_t BoxPara[4];                                                               //油箱参数
  uint16_t AddLiqTime;                                                               //加油时间阈值
  uint16_t SubLiqTime;                                                               //漏油时间阈值
  uint16_t AddLiqCnt;                                                                //加油量阈值
  uint16_t SubLiqCnt;                                                                //漏油量阈值
  uint16_t RestLiqCnt;                                                                //二次加油阈值
  uint16_t AddOil;                                                                   //加油量
  uint16_t SubOil;                                                                   //漏油量
  uint16_t CalibTemp;                                                                //标定温度 保留 暂时无用
  uint32_t OilRange;                                                                 //油量量程
  uint32_t Oil_Add_Alarm;                                                            //加油极限报警
  uint32_t Oil_AddMinAlarm;                                                          //加油最小报警点
  FloaterCalibTypeDef Floater;                                                  //油浮标定类型结构
}ProductParaTypeDef;

typedef enum 
{
  Def_State = 0x00,                                                             //出厂标定状态
  CalibrateLow = 0x01,                                                          //标定零点
  CalibrateHeight = 0x02,                                                       //标定满度
  Calibrated = 0x03,                                                            //标定完成
}CalState_TypeDef;

typedef struct
{
  uint8_t OilInit;                                                                   //油量初始化标志
  uint8_t AccStatus;                                                                 //ACC状态
  uint16_t CarSpeed;                                                                 //汽车速度
  uint16_t FiltSpeed;                                                                //滤波速度
  uint16_t LowSpeedCount;                                                            //持续低速计数
  uint16_t DAForFloater;                                                             //油浮DA值
  uint32_t CarMileage;                                                               //里程
  uint32_t CapFromPCap;                                                              //电容
  uint32_t RawCap;                                                                   //原始电容
  uint32_t RawCapBak;                                                                //原始电容备份

  uint32_t OilQuantity;                                                              //邮箱油量
  uint32_t LiquidHeight;                                                             //液位高度值mm
  uint16_t LiquidAD;                                                                 //液位高度AD
  uint16_t LiquidPercent;                                                            //液位百分比
  float TempInSensor;                                                           //液体温度
  int TempInAir;                                                              //环境温度
  float HeightBak;
  
  CalState_TypeDef CalState;                                                    //标定状态结构体参数
 // OS_TICK uartInitTime;                                                         //Uart初始化时间
  BitAction bInitializing;
}RunVarTypeDef;

typedef struct
{
  uint8_t FilterLevel;                                                               //滤波等级
  uint8_t FilterBufMax;                                                              //滤波缓存最大值
  uint8_t FilterN;                                                                   //液位高度下限位（低于该值认为小波动，数据可更新）
  uint8_t FilterM;                                                                   //液位高度上限位（高于该值认为大波动，数据可更新）
  uint16_t FilterCycle;                                                              //滤波周期
  uint32_t HFil[10];                                                                 //高阶滤波数组
  uint32_t HFilBak[10];                                                              //高阶滤波数组备份
  uint32_t LFil[96];                                                                 //低阶滤波数组
  uint32_t LFilBak[96];                                                              //低阶滤波数组备份        
  uint32_t PCap_Filter;                                                              //滤波后的电容值
  float UserFlowK;                                                             //用户系数K
  float UserFlowB;                                                             //用户系数B
}UserParamTypeDef;



typedef struct
{
    BitAction FilterStart;                                                      //每秒滤波开始标志                                                           
    uint8_t Ms100_Cycle;                                                             //每1s获得电容次数计数                        
    uint32_t EverySecCap;                                                            //每秒的电容值                
    uint32_t FilArray[10];                                                           //每秒电容滤波数组
}EverySecFilTypeDef;


#endif

