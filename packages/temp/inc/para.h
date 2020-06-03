#ifndef _PARA_H
#define _PARA_H
#include "stm32l0xx.h"

typedef struct 
{
    uint32_t Temp;           //�¶�ֵ*10000 
    uint8_t SlaveAddr;
    //uint8_t SenSta;     //������״̬  �ò���      
    uint16_t Up_Thr;    //�¶��Ϸ�ֵ  
    uint16_t Do_Thr;    //�¶��·�ֵ
    uint16_t Du_Thr;    //����ʱ�䷧ֵ������������ʱ��Ž���״̬�л���   
    uint32_t Duration;  //���������¶���ֵ����ʱ��
    uint32_t AlarmSta;  //����״̬
}UserTypeDef;
//#define EEPROM_BASE_ADDR	0x08080000	
//#define EEPROM_BYTE_SIZE	0x0FFF
#define RUN_ADDR_BASE	    			                  0x00								//������ʼ����־λ������ÿ���ϵ綼дEEPROMP
//#define EEPROM_BASE_ADDR	    			                 0x08080000//  0x00								//������ʼ����־λ������ÿ���ϵ綼дEEPROMP   
#define SLAVE_ADDR					(RUN_ADDR_BASE		+ 0x01)
#define TEM_UP_THR                                      (SLAVE_ADDR             + 0x01) //�¶�1�Ϸ�ֵ
#define TEM_DO_THR                                      (TEM_UP_THR             + 0x02)
#define TEM_DU_THR                                      (TEM_DO_THR             + 0x02)
#define USER_DEFAULT_LEN_T				(TEM_DU_THR             + 0x02)

//---------------------------------------------------
#define COMPANY_T						(USER_DEFAULT_LEN_T       + 0x40) 
#define DEV_ENCODING_T				        (COMPANY_T		+ 0x40)
#define HWVERSION					(DEV_ENCODING_T		+ 0x40)
#define SFVERSION					(HWVERSION	        + 0x40)
#define DEV_ID_T						(SFVERSION		+ 0x40)
#define CUSTOMERCODE_temp                                    (DEV_ID_T		        + 0x40)  


void ReadPara(void);


#endif
