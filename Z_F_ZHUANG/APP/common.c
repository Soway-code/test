#include "common.h"

void Delay_Ms(uint32_t cnt)
{
    cnt = cnt * 3200;
   

    while (cnt--);
}

void Delay_Us(uint32_t cnt)
{
    cnt = cnt * 8;
    //cnt = cnt * 2;
    while (cnt--);
}

void delay_us(uint32_t us)
{
    uint32_t delay = (HAL_RCC_GetHCLKFreq() / 4000000 * us);
    while(delay--)
    {
        ;
    }

}


//**************************************************************************************************
// ����         : Unshort2Array()
// ��������     : 2018-06-08
// ����         : MMX
// ����         : unsigned short��������ת��Ϊ����
// �������     : uint16_t SourceData       Ҫת��������
// �������     : uint8_t* Array            ת��������
// ���ؽ��     : ��
// ע���˵��   : ��
// �޸�����     : �� 
//**************************************************************************************************
void Unshort2Array(uint16_t SourceData, uint8_t* Array)
{
    *Array = *((uint8_t*)&SourceData + 1);
    *(Array + 1) = *(uint8_t*)&SourceData;
}



//**************************************************************************************************
// ����         : long32Array()
// ��������     : 2018-06-08
// ����         : MMX
// ����         : long��������ת��Ϊ����
// �������     : uint32_t SourceData       Ҫת��������
// �������     : uint8_t* Array            ת��������
// ���ؽ��     : ��
// ע���˵��   : ��
// �޸�����     : �� 
//**************************************************************************************************
void long32Array(uint32_t SourceData, uint8_t* Array)
{
  //  *Array = *((uint8_t*)&SourceData + 1);
   // *(Array + 1) = *(uint8_t*)&SourceData;
    
   *Array = *((uint8_t*)&SourceData + 3);
   *(Array + 1) = *((uint8_t*)&SourceData + 2);
   *(Array + 2) = *((uint8_t*)&SourceData + 1);
   *(Array + 3) = *(uint8_t*)&SourceData;

}


//**************************************************************************************************
// ����         : Unlong2Array()
// ��������     : 2018-06-08
// ����         : MMX
// ����         : uint32_t��������ת��Ϊ����
// �������     : uint32_t SourceData       Ҫת��������
// �������     : uint8_t* Array            ת��������
// ���ؽ��     : ��
// ע���˵��   : ��
// �޸�����     : �� 
//**************************************************************************************************
void Unlong2Array(uint32_t SourceData, uint8_t* Array)
{
    uint8_t i;
    for(i = 0; i < 4; i++)
    {
        *(Array + i) = *((uint8_t*)&SourceData + 3 - i);
    }
}




