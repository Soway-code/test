/**@file        TypeConvert.c
* @brief        ��������ת��ģ��
* @details      �����������븡�������ݻ�Ϊת��,���������ƽ��ֵ
* @author       ���
* @date         2020-04-29
* @version      V1.0.0
* @copyright    2020-2030,��������Ϊ�Ƽ���չ���޹�˾
**********************************************************************************
* @par �޸���־:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/04/29  <td>1.0.0    <td>���    <td>������ʼ�汾
* </table>
*
**********************************************************************************
*/

#include "TypeConvert.h"


/**@brief       ��32λ���ͱ���(���ģʽ)ת���ɸ�����ֵ(С��ģʽ)
* @param[in]    pData : �޷����ַ���ָ��
* @return       ����ִ�н��
* - ת����ĸ�������ֵ
*/
float HexToFloat(uint8_t *pData)
{
    uint32_t Data;
    Data = pData[0];
    Data <<= 8;
    Data += pData[1];
    Data <<= 8;
    Data += pData[2];
    Data <<= 8;
    Data += pData[3];
    return *(float*)&Data;
}

/**@brief       ��32λ���ͱ���(���ģʽ)ת�����޷���32λ����ֵ(С��ģʽ)
* @param[in]    pData : �޷����ַ���ָ��
* @return       ����ִ�н��
* - ת������޷���32λ����ֵ
*/
uint32_t HexToUlong(uint8_t *pData)
{
    uint32_t Data;
    Data = pData[0];
    Data <<= 8;
    Data += pData[1];
    Data <<= 8;
    Data += pData[2];
    Data <<= 8;
    Data += pData[3];
    return Data;
}

/**@brief       �������ͱ���(С��ģʽ)ת�����޷���8λ����ֵ������(���ģʽ)
* @param[in]    fSource : �����ͱ���; 
* @param[in]    num : ������ֽڸ���
* @param[out]   pu8Temp : ת����Ľ��
* @return       ����ִ�н��
* - None
*/
void floatToHexArray(float fSource, uint8_t *pu8Temp, uint16_t num)
{
    uint16_t i;
    uint32_t u32Source = *(uint32_t *)&fSource;

    for(i = 0; i < num; i++)
    {
        pu8Temp[num - i - 1] = (u32Source >> (8 * i)) & 0xFF;
    }
}

/**@brief       ��Array[]���鰴ָ��ͷ����SortHeadSize��β����SortTailSize����˳������
* @param[in]    Array[] : Ҫ���е�����;           
* @param[in]    ArraySize : ���鳤��;
* @param[in]    SortHeadSize : ����ͷ�����г���;
* @param[in]    SortTailSize : ����β�����г���;
* @return       ����ִ�н��
* - None
*/
void SortArrayExtreme(uint32_t Array[], uint32_t ArraySize, uint32_t SortHeadSize, uint32_t SortTailSize)
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

/**@brief       ����Array[]�����Array[DelHeadSize]��Array[ArraySize - DelTailSize - 1]��ƽ��ֵ
* @param[in]    Array[] : Ҫ����ƽ��ֵ������;           
* @param[in]    ArraySize : ���鳤��;
* @param[in]    DelHeadSize : ����ͷ���޳��ĳ���;
* @param[in]    DelTailSize : ����β���޳��ĳ���;
* @return       ����ִ�н��
* - Array[DelHeadSize]��Array[ArraySize - DelTailSize - 1]��ƽ��ֵ
*/
uint32_t GetAverage(uint32_t Array[], uint32_t ArraySize, uint32_t DelHeadSize, uint32_t DelTailSize)
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

/**@brief       ����Array[]���鰴ָ��ͷ����SortHeadSize��β����SortTailSize����˳�����к�,
* ��Array[DelHeadSize]��Array[ArraySize - DelTailSize - 1]��ƽ��ֵ
* @param[in]    Array[] : Ҫ�������в�����ƽ��ֵ������;           
* @param[in]    ArraySize : ���鳤��;
* @param[in]    SortHeadSize : ����ͷ�����г���,����ƽ��ֵʱ����ͷ���޳��ĳ���;
* @param[in]    SortTailSize : ����β�����г���,����ƽ��ֵʱ����β���޳��ĳ���;
* @return       ����ִ�н��
* - Array[]���鰴ָ��ͷ����SortHeadSize��β����SortTailSize����˳�����к�,
* ��Array[DelHeadSize]��Array[ArraySize - DelTailSize - 1]��ƽ��ֵ
*/
uint32_t GetDelExtremeAndAverage(uint32_t Array[], uint32_t ArraySize, uint32_t SortHeadSize, uint32_t SortTailSize)
{
    SortArrayExtreme(Array, ArraySize, SortHeadSize, SortTailSize);
    return(GetAverage(Array, ArraySize, SortHeadSize, SortTailSize));
}

