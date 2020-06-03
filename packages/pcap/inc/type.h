#ifndef __TYPE_H
#define __TYPE_H

#include "stm32l0xx.h"	
#include "filter.h"

//#include "Sys_cfg.h"
//#include "os.h"

#define ECHO_ENABLE 	        0                                               //����
#define ECHO_DISABLE 	        1                                               //�ⶳ

#define FLASH_WR_ENABLE         1                                               //Flashдʹ��
#define FLASH_WR_DISABLE        0                                               //Flashд��ֹ

typedef struct 
{
  uint8_t SlaveAddr;                                                                 //�ӻ���ַ
  uint8_t BaudRate;                                                                  //������
  uint8_t WordLength;                                                                //����λ
  uint8_t StopBits;                                                                  //ֹͣλ
  uint8_t Parity;                                                                    //У��λ
  uint8_t Echo;                                                                      //�Ƿ񶳽�
}UartCommTypeDef;

typedef struct
{
  uint16_t DACode[8];                                                                //DA��
  uint16_t Scale[8];                                                                 //�ٷֶ�
}FloaterCalibTypeDef;

typedef struct
{
  uint32_t CapMin;                                                                   //�������
  uint32_t CapMax;                                                                   //��������
  uint32_t CapRange;                                                                 //���ݷ�Χ       
  uint16_t Range;                                                                    //����
  uint16_t SensorLen;                                                                 //����������
  float DAMin;                                                                  //DA���
  float DAMax;                                                                  //DA����
  float Def_Range;                                                              //��������
  float Def_CapMin;                                                             //�������
  float Def_CapMax;                                                             //��������
  
  float Temp_K;                                                                 //�¶�1ϵ��K
  float Temp_B;                                                                 //�¶�1ϵ��B
  float Temp2_K;                                                                //�¶�2ϵ��K
  float Temp2_B;                                                                //�¶�2ϵ��B
  float LiquitHeightforDAMax;                                                   //Һλ���DAֵ
  float BottomArea;                                                             //�����(��λ��ƽ����) 
  float AddMapHeight;                                                           //������ֵ��ӳ�����ĸ߶ȣ���λ��mm��
  float SubMapHeight;                                                           //©����ֵ��ӳ�����ĸ߶ȣ���λ��mm��
  uint8_t AutoUpLoadTime;                                                             //�Զ��ϴ�����
  uint8_t CompenEn;                                                                   //����ʹ��
  //u8 OutputUnit;
  uint8_t bFlashWEn;                                                                 //дFlashʹ��
  uint8_t LiquidCmpTc;                                                               //Һλ�Ƚ�ʱ���� 2 3 4 5
  uint8_t FuelType;                                                                  //ȼ������
  uint8_t BoxShape;                                                                  //������״
  uint16_t BoxPara[4];                                                               //�������
  uint16_t AddLiqTime;                                                               //����ʱ����ֵ
  uint16_t SubLiqTime;                                                               //©��ʱ����ֵ
  uint16_t AddLiqCnt;                                                                //��������ֵ
  uint16_t SubLiqCnt;                                                                //©������ֵ
  uint16_t RestLiqCnt;                                                                //���μ�����ֵ
  uint16_t AddOil;                                                                   //������
  uint16_t SubOil;                                                                   //©����
  uint16_t CalibTemp;                                                                //�궨�¶� ���� ��ʱ����
  uint32_t OilRange;                                                                 //��������
  uint32_t Oil_Add_Alarm;                                                            //���ͼ��ޱ���
  uint32_t Oil_AddMinAlarm;                                                          //������С������
  FloaterCalibTypeDef Floater;                                                  //�͸��궨���ͽṹ
}ProductParaTypeDef;

typedef enum 
{
  Def_State = 0x00,                                                             //�����궨״̬
  CalibrateLow = 0x01,                                                          //�궨���
  CalibrateHeight = 0x02,                                                       //�궨����
  Calibrated = 0x03,                                                            //�궨���
}CalState_TypeDef;

typedef struct
{
  uint8_t OilInit;                                                                   //������ʼ����־
  uint8_t AccStatus;                                                                 //ACC״̬
  uint16_t CarSpeed;                                                                 //�����ٶ�
  uint16_t FiltSpeed;                                                                //�˲��ٶ�
  uint16_t LowSpeedCount;                                                            //�������ټ���
  uint16_t DAForFloater;                                                             //�͸�DAֵ
  uint32_t CarMileage;                                                               //���
  uint32_t CapFromPCap;                                                              //����
  uint32_t RawCap;                                                                   //ԭʼ����
  uint32_t RawCapBak;                                                                //ԭʼ���ݱ���

  uint32_t OilQuantity;                                                              //��������
  uint32_t LiquidHeight;                                                             //Һλ�߶�ֵmm
  uint16_t LiquidAD;                                                                 //Һλ�߶�AD
  uint16_t LiquidPercent;                                                            //Һλ�ٷֱ�
  float TempInSensor;                                                           //Һ���¶�
  int TempInAir;                                                              //�����¶�
  float HeightBak;
  
  CalState_TypeDef CalState;                                                    //�궨״̬�ṹ�����
 // OS_TICK uartInitTime;                                                         //Uart��ʼ��ʱ��
  BitAction bInitializing;
}RunVarTypeDef;

typedef struct
{
  uint8_t FilterLevel;                                                               //�˲��ȼ�
  uint8_t FilterBufMax;                                                              //�˲��������ֵ
  uint8_t FilterN;                                                                   //Һλ�߶�����λ�����ڸ�ֵ��ΪС���������ݿɸ��£�
  uint8_t FilterM;                                                                   //Һλ�߶�����λ�����ڸ�ֵ��Ϊ�󲨶������ݿɸ��£�
  uint16_t FilterCycle;                                                              //�˲�����
  uint32_t HFil[10];                                                                 //�߽��˲�����
  uint32_t HFilBak[10];                                                              //�߽��˲����鱸��
  uint32_t LFil[96];                                                                 //�ͽ��˲�����
  uint32_t LFilBak[96];                                                              //�ͽ��˲����鱸��        
  uint32_t PCap_Filter;                                                              //�˲���ĵ���ֵ
  float UserFlowK;                                                             //�û�ϵ��K
  float UserFlowB;                                                             //�û�ϵ��B
}UserParamTypeDef;



typedef struct
{
    BitAction FilterStart;                                                      //ÿ���˲���ʼ��־                                                           
    uint8_t Ms100_Cycle;                                                             //ÿ1s��õ��ݴ�������                        
    uint32_t EverySecCap;                                                            //ÿ��ĵ���ֵ                
    uint32_t FilArray[10];                                                           //ÿ������˲�����
}EverySecFilTypeDef;


#endif

