/**
* @file         inflash.h
* @details      inflash.c的头文件,声明了读写内部Flash驱动程序的API函数,
* 定义了操作 Flash 的基地址和可操作的地址范围
* @author       韦剑祥
* @date         2020-08-17
* @version      V1.0.0
* @copyright    2020-2030,深圳市信为科技发展有限公司
**********************************************************************************
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/08/17  <td>1.0.0    <td>韦剑祥    <td>创建初始版本
* </table>
*
**********************************************************************************
*/

#ifndef _INFLASH_H
#define _INFLASH_H

#include "stm32g0xx_hal.h"


#define  OP_FAILED				-1
#define  OP_SUCCESS				0

#define IN_FLASH_BASE_ADDRESS           0x0801E800                          ///< 操作Flash基地址

#define IN_FLASH_START                  0x0000                              ///< Flash起始地址
#define IN_FLASH_END                    0x17FF                              ///< Flash结束地址

#define IN_FLASH_WR_ENABLE              0x0F                                ///< Flash写操作使能
#define IN_FLASH_WR_DISABLE             0x00                                ///< Flash写操作禁止

#define WRITE_FLASH_ERR_MAX             0x03                                ///< 写Flash错误最大次数

#define FLASH_TYPEPROGRAM_WORD       (0x02U)  /*!<Program a word (32-bit) at a specified address.*/
/**@brief       向内部Flash指定位置写入一字节数据
* @param[in]    RWAddr : 写入地址
* @param[in]    WrData : 写入数据
* @return       函数执行结果
* - OP_SUCCESS(成功)
* - OP_FAILED(失败)
* @note         地址不能越界,最大值为 FLASH_END
*/
uint8_t InFlash_Write_OneByte(uint16_t RWAddr, uint8_t WrData);

/**@brief       向内部Flash指定位置读取一字节数据
* @param[in]    RWAddr : 读取地址
* @return       函数执行结果
* - 1个字节数据
* @note         地址不能越界,最大值为 FLASH_END
*/
uint8_t InFlash_Read_OneByte(uint16_t RWAddr);

/**@brief       向内部Flash指定位置写多个字节
* @param[in]    RWAddr : 写起始地址
* @param[in]    pWrbuf : 数据缓存指针
* @param[in]    Wrlen : 写数据长度
* @return       函数执行结果
* - OP_SUCCESS(成功)
* - OP_FAILED(失败)
* @note         地址不能越界,最大值为 FLASH_END
*/
uint8_t InFlash_Write_MultiBytes(uint16_t RWAddr, uint8_t const *pWrbuf, uint16_t Wrlen);

/**
* @brief        		Flash 写N个双字
* @param[in]     		: Address -- 写数据首地址
						Data -- 写数据指针
						DataLen -- 写数据长度
* @retval	     		None
*/
uint8_t FlashWriteDoubleWord(uint32_t Address, uint64_t *Data, uint16_t DataLen);

/**@brief       从内部Flash指定位置读多个字节
* @param[in]    RWAddr : 读起始地址
* @param[in]    pRdbuf : 数据缓存指针
* @param[in]    Rdlen : 读数据长度
* @return       函数执行结果
* - None
* @note         地址不能越界,最大值为 FLASH_END
*/
void InFlash_Read_MultiBytes(uint16_t RWAddr, uint8_t *pRdbuf, uint16_t Rdlen);

/**@brief       向内部Flash指定位置擦除页
* @param[in]    RWAddr : 擦除起始地址
* @param[in]    PageNb : 擦除页数目
* @return       函数执行结果
* - OP_SUCCESS(成功)
* - OP_FAILED(失败)
* @note         地址不能越界,最大值为 FLASH_END
*/
uint8_t InFlash_Erase_Page(uint16_t RWAddr, uint8_t PageNb);

#endif




