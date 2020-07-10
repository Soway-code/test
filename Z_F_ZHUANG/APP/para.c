#include "para.h"
#include "type.h"
#include "inflash.h"

UserTypeDef UserPara;
uint8_t Cur_Param[USER_DEFAULT_LEN];
//extern uint16_t LiquidUnit;
//extern uint8_t  LiquidRange;
extern uint8_t time_tick;

uint32_t Current_pulse = 0;  //��ǰ������
uint32_t Current_PositiveTime = 0;  //��ǰ��ת������
uint32_t Current_NegativeTime = 0;  //��ǰ��ת������

//���ݵĸ��ֽ���ǰ�����ֽ��ں󣨴��ģʽ��
uint8_t  User_Default_Param[USER_DEFAULT_LEN] =
{
    0x01,                       //������־
    0x51,                       //�豸ID        
    0x03,                       //������
    0x03,                       //��żУ��λ
    
    0x00,0x00,0x00,0x00,           //������ʱ���ֵ
    0x00,0x00,0x00,0x00,          //ֹͣʱ���ֵ
    0x00,0x00,0x00,0x00,           //��תʱ���ֵ
    0x00,0x00,0x00,0x00,           //��תʱ���ֵ
    0x00,0x00,0x00,0x00,           //��������ֵ
    0x00,0x00,0x00,0x00            //��ǰ״̬����ʱ��
    
};


uint16_t ArrToHex(uint8_t *pData)
{
    uint16_t Data;
    Data = pData[0];
    Data <<= 8;
    Data += pData[1];
 
    return *(uint16_t*)&Data;
}




void SortArrayExtreme(uint32_t Array[], const uint32_t ArraySize,
                      const uint32_t SortHeadSize, const uint32_t SortTailSize)
{
    uint32_t i, j;
    uint32_t temp;

    for (i = 0; i < SortTailSize; i++)
    {
        for (j = 0; j < ArraySize - i - 1; j++)
        {
            if (Array[j] > Array[j+1])
            {
                temp = Array[j];
                Array[j] = Array[j+1];
                Array[j+1] = temp;
            }
        }
    }

    for (i = 0; i < SortHeadSize; i++)
    {
        for (j = ArraySize - SortTailSize - 1 ; j > i; j--)
        {
            if (Array[j - 1] > Array[j])
            {
                temp = Array[j - 1];
                Array[j - 1] = Array[j];
                Array[j] = temp;
            }
        }
    }
}

uint32_t GetAverage(uint32_t Array[], const uint32_t ArraySize,
               const uint32_t DelHeadSize, const uint32_t DelTailSize)
{
    uint64_t sum = 0;

    if ((DelHeadSize + DelTailSize) >= ArraySize)
    {
        return 0;
    }

    for (long i = DelHeadSize; i < ArraySize - DelTailSize; i++)
    {
        sum += Array[i];
    }

    return(sum / (ArraySize - DelHeadSize - DelTailSize));
}


uint32_t GetDelExtremeAndAverage(uint32_t Array[], const uint32_t ArraySize,
                            const uint32_t SortHeadSize, const uint32_t SortTailSize)
{
    SortArrayExtreme(Array, ArraySize, SortHeadSize, SortTailSize);
    return(GetAverage(Array, ArraySize, SortHeadSize, SortTailSize));
}

void Switch_Fiter(uint8_t value)
{
    switch(value)
    {	
    case 1:
        UserPara.FilterBufMax = 1;  
        time_tick = 100;
       // UserPara.FilterCycle = 1;      //ʵʱ�˲�1s
        break;

    case 2:	
        UserPara.FilterBufMax = 18;  
        time_tick = 100;
       // UserPara.FilterCycle = 180;    //ƽ���˲�180s
        break;

    case 3:	
        UserPara.FilterBufMax = 36;    //ƽ���˲�960s
        time_tick = 10;
       // UserPara.FilterCycle = 960;
        break;

    default:
        UserPara.FilterBufMax = 18;    
        //UserPara.FilterCycle = 180;    //ʵʱ�˲�1s
        break;
    }
}

//******************************************************************************
// ����         : ReadPara()
// ��������     : 2018-06-08
// ����         : MMX
// ����         : ��ȡEEPROM����
// �������     : ��
// �������     : ��
// ���ؽ��     : ��
// ע���˵��   : ��
// �޸�����     : �� 
//******************************************************************************
void ReadPara(void)
{
    uint8_t ParaInitFlag,i;
    uint8_t uTemp[4];
    
//    Eeprom_ReadNBytes(COMMON_BASE_ADDR, &ParaInitFlag, 1);
	FlashReadNWord(COMMON_BASE_ADDR, (uint32_t*)&ParaInitFlag, 1);
    
    if(ParaInitFlag != User_Default_Param[0])
    {
		FlashWriteDoubleWord(RUN_ADDR_BASE, (uint64_t*)User_Default_Param, USER_DEFAULT_LEN);
//        Eeprom_WriteNBytes(COMMON_BASE_ADDR, User_Default_Param, USER_DEFAULT_LEN);
    }
    
//    Eeprom_ReadNBytes(COMMON_BASE_ADDR, Cur_Param, USER_DEFAULT_LEN);
	FlashReadNWord(COMMON_BASE_ADDR, (uint32_t*)Cur_Param, USER_DEFAULT_LEN);
    
    
    UserPara.SlaveAddr = Cur_Param[SLAVE_ADDR];  //�ӻ���ַ
    //UserPara.SlaveAddr = Cur_Param[1];  //�ӻ���ַ
      
    UserPara.Baudrate = Cur_Param[BAUDRATE];  //������
    
    UserPara.Parity = Cur_Param[PARITY];    //��żУ��
    
     for(i = 0; i < 4 ;i ++)
    {
        uTemp[i] = Cur_Param[WORK_TIME_BASE + i];
        UserPara.WorkTime += ((uint32_t)uTemp[i] << (3 - i) * 8); 
    }
    
     for(i = 0; i < 4 ;i ++)
    {
        uTemp[i] = Cur_Param[NEGATIVE_ROTATE_TIME_BASE + i];
        UserPara.NegativeTimeBase += ((uint32_t)uTemp[i] << (3 - i) * 8); 
    }
    Current_NegativeTime = UserPara.NegativeTimeBase;
    
     for(i = 0; i < 4 ;i ++)
    {
        uTemp[i] = Cur_Param[POSITIVE_ROTATE_TIME_BASE + i];
        UserPara.PositiveTimeBase += ((uint32_t)uTemp[i] << (3 - i) * 8); 
    }
    Current_PositiveTime =  UserPara.PositiveTimeBase;
    
     for(i = 0; i < 4 ;i ++)
    {
        uTemp[i] = Cur_Param[PULSE_TOTAL_BASE + i];
        UserPara.TotalPulse += ((uint32_t)uTemp[i] << (3 - i) * 8); 
    }
    Current_pulse  = UserPara.TotalPulse;
    

}




