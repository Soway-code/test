/**@file        TypeConvert.h
* @details      TypeConvert.c的头文件,声明了数据类型转换模块的API函数
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

#ifndef __TYPECONVERT_H
#define __TYPECONVERT_H

#include "stdint.h"

/**@brief       将32位整型变量(大端模式)转换成浮点型值(小端模式)
* @param[in]    pData : 无符号字符型指针
* @return       函数执行结果
* - 转换后的浮点型数值
*/
float HexToFloat(uint8_t *pData);

/**@brief       将32位整型变量(大端模式)转换成无符号32位整型值(小端模式)
* @param[in]    pData : 无符号字符型指针
* @return       函数执行结果
* - 转换后的无符号32位整型值
*/
uint32_t HexToUlong(uint8_t *pData);

/**@brief       将浮点型变量(小端模式)转换成无符号8位整型值的数组(大端模式)
* @param[in]    fSource : 浮点型变量; 
* @param[in]    num : 输入的字节个数
* @param[out]   pu8Temp : 转换后的结果
* @return       函数执行结果
* - None
*/
void floatToHexArray(float fSource, uint8_t *pu8Temp, uint16_t num);

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
uint32_t GetDelExtremeAndAverage(uint32_t Array[], uint32_t ArraySize, uint32_t SortHeadSize, uint32_t SortTailSize);

#endif
