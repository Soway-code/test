#include "para.h"
//#include "type.h"
#include "eeprom.h"
#include "stm32l0xx_hal_flash.h"

UserTypeDef UserPara;
uint8_t Temp_Cur_Param[USER_DEFAULT_LEN_T];

static uint8_t User_Default_Param[USER_DEFAULT_LEN_T] =
{
    0x01,
    0x21,
    0xF4,0x0B,          //���ֽ���ǰ��4031�൱��130�棬�¶ȱ�������ֵ 0xBF,0x0F, 
    0x47,0x0A,          //���ֽ���ǰ��2231�൱��-50�棬�¶ȱ�������ֵ        
    0xB7,0x08,          //���ֽ���ǰ��30s������ʱ����ֵ         
};

void ReadPara(void)
{
    uint8_t ParaInitFlag;
   // Eeprom_ReadNBytes(RUN_ADDR_BASE, &ParaInitFlag, 1);
   //rt_kprintf("ReadPara_comin\n");
		Eeprom_ReadNBytes(EEPROM_TEMP_ADDR, &ParaInitFlag, 1);
		ParaInitFlag =0x02;
    if(ParaInitFlag != User_Default_Param[0])
    {
				Eeprom_WriteNBytes(EEPROM_TEMP_ADDR, User_Default_Param, USER_DEFAULT_LEN_T);
    }
    //Eeprom_ReadNBytes(RUN_ADDR_BASE, Temp_Cur_Param, USER_DEFAULT_LEN);
		Eeprom_ReadNBytes(EEPROM_TEMP_ADDR, Temp_Cur_Param, USER_DEFAULT_LEN_T);
    UserPara.SlaveAddr = Temp_Cur_Param[1];
    UserPara.Up_Thr = ((uint16_t)Temp_Cur_Param[3] << 8) +Temp_Cur_Param[2];
    UserPara.Do_Thr = ((uint16_t)Temp_Cur_Param[5] << 8) +Temp_Cur_Param[4];
    UserPara.Du_Thr = ((uint16_t)Temp_Cur_Param[7] << 8) +Temp_Cur_Param[6];
}
