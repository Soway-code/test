/**@file        TypeConvert.c
* @brief        数据类型转换模块
* @details      将整型数据与浮点型数据互为转换,计算数组的平均值
* @author       杨春林
* @date         2020-04-29
* @version      V1.0.0
* @copyright    2020-2030,深圳市信为科技发展有限公司
**********************************************************************************
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/04/29  <td>1.0.0    <td>杨春林    <td>创建初始版本
* </table>
*
**********************************************************************************
*/

#include "TypeConvert.h"


/**@brief       将32位整型变量(大端模式)转换成浮点型值(小端模式)
* @param[in]    pData : 无符号字符型指针
* @return       函数执行结果
* - 转换后的浮点型数值
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

/**@brief       将32位整型变量(大端模式)转换成无符号32位整型值(小端模式)
* @param[in]    pData : 无符号字符型指针
* @return       函数执行结果
* - 转换后的无符号32位整型值
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

/**@brief       将浮点型变量(小端模式)转换成无符号8位整型值的数组(大端模式)
* @param[in]    fSource : 浮点型变量; 
* @param[in]    num : 输入的字节个数
* @param[out]   pu8Temp : 转换后的结果
* @return       函数执行结果
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

/**@brief       对Array[]数组按指定头长度SortHeadSize和尾长度SortTailSize进行顺序排列
* @param[in]    Array[] : 要排列的数组;           
* @param[in]    ArraySize : 数组长度;
* @param[in]    SortHeadSize : 数组头部排列长度;
* @param[in]    SortTailSize : 数组尾部排列长度;
* @return       函数执行结果
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

/**@brief       计算Array[]数组从Array[DelHeadSize]到Array[ArraySize - DelTailSize - 1]的平均值
* @param[in]    Array[] : 要计算平均值的数组;           
* @param[in]    ArraySize : 数组长度;
* @param[in]    DelHeadSize : 数组头部剔除的长度;
* @param[in]    DelTailSize : 数组尾部剔除的长度;
* @return       函数执行结果
* - Array[DelHeadSize]到Array[ArraySize - DelTailSize - 1]的平均值
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

/**@brief       计算Array[]数组按指定头长度SortHeadSize和尾长度SortTailSize进行顺序排列后,
* 从Array[DelHeadSize]到Array[ArraySize - DelTailSize - 1]的平均值
* @param[in]    Array[] : 要进行排列并计算平均值的数组;           
* @param[in]    ArraySize : 数组长度;
* @param[in]    SortHeadSize : 数组头部排列长度,计算平均值时数组头部剔除的长度;
* @param[in]    SortTailSize : 数组尾部排列长度,计算平均值时数组尾部剔除的长度;
* @return       函数执行结果
* - Array[]数组按指定头长度SortHeadSize和尾长度SortTailSize进行顺序排列后,
* 从Array[DelHeadSize]到Array[ArraySize - DelTailSize - 1]的平均值
*/
uint32_t GetDelExtremeAndAverage(uint32_t Array[], uint32_t ArraySize, uint32_t SortHeadSize, uint32_t SortTailSize)
{
    SortArrayExtreme(Array, ArraySize, SortHeadSize, SortTailSize);
    return(GetAverage(Array, ArraySize, SortHeadSize, SortTailSize));
}

