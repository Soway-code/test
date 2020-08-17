/**@file        flash.h
* @details      flash.c的头文件,声明了flash应用的API函数
* @author       马灿林
* @date         2020-08-14
* @version      V1.0.0
* @copyright    2020-2030,深圳市信为科技发展有限公司
**********************************************************************************
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/08/14  <td>1.0.0    <td>马灿林    <td>创建初始版本
* </table>
*
**********************************************************************************
*/

#ifndef __FLASH_H__
#define __FLASH_H__

#include "main.h"

#define FLASH_BASE_ADDRESS           0x8010000
extern FLASH_EraseInitTypeDef EraseInitStruct;
uint32_t GetPage(uint32_t Addr);
uint8_t FlashWriteDoubleWord(uint32_t Address, uint64_t *Data, uint16_t DataLen);
HAL_StatusTypeDef FlashPageErase(uint32_t Address);

#endif
