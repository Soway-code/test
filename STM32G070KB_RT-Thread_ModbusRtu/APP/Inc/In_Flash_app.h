/**@file        In_Flash_app.h
* @details      In_Flash_app.c的头文件,声明了内部Flash应用的API函数
* @author       杨春林
* @date         2020-04-30
* @version      V1.0.0
* @copyright    2020-2030,深圳市信为科技发展有限公司
**********************************************************************************
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/04/30  <td>1.0.0    <td>杨春林    <td>创建初始版本
* </table>
*
**********************************************************************************
*/
#ifndef __IN_FLASH_APP_H
#define __IN_FLASH_APP_H

#include "In_Flash.h"
#include "TypeConvert.h"

/***************************** 设备参数在内部Flash中的映射地址 ****************************/
#define RUN_ADDR_BASE                     0x00                                  ///< 系统参数初始化标志/内部Flash起始地址
#define DEVICE_ADDR                       (RUN_ADDR_BASE + 0x01)                ///< 设备地址
#define BAUDRATE                          (DEVICE_ADDR   + 0x01)                ///< 波特率
#define PARITY                            (BAUDRATE      + 0x01)                ///< 奇偶校验
#define FILTER                            (PARITY        + 0x01)                ///< 滤波系数
#define AUTO_UPLOAD                       (FILTER        + 0x01)                ///< 自动上传周期
#define COMPENSATE                        (AUTO_UPLOAD   + 0x01)                ///< 补偿使能
#define FREEZE                            (COMPENSATE    + 0x01)                ///< 是否冻结设备
#define OUTPUTMODE                        (FREEZE        + 0x01)                ///< 输出方式
#define HEIGHTRANGE                       (OUTPUTMODE    + 0x01)                ///< 量程
#define CAPMIN                            (HEIGHTRANGE   + 0x02)                ///< 零点电容
#define CAPMAX                            (CAPMIN        + 0x04)                ///< 满量程电容
#define CAPMINBAK                         (CAPMAX        + 0x04)                ///< 零点电容备份
#define CAPMAXBAK                         (CAPMINBAK     + 0x04)                ///< 满量程电容备份
#define CAPADMIN                          (CAPMAXBAK     + 0x04)                ///< 电容零点AD值
#define CAPADLOW                          (CAPADMIN      + 0x02)                ///< 电容下刻度AD值
#define CAPADHIGH                         (CAPADLOW      + 0x02)                ///< 电容上刻度AD值
#define CAPADMAX                          (CAPADHIGH     + 0x02)                ///< 电容满量程AD值
#define CAPDAMIN                          (CAPADMAX      + 0x02)                ///< 电容零点DA值
#define CAPDALOW                          (CAPDAMIN      + 0x02)                ///< 电容下刻度DA值
#define CAPDAHIGH                         (CAPDALOW      + 0x02)                ///< 电容上刻度DA值
#define CAPDAMAX                          (CAPDAHIGH     + 0x02)                ///< 电容满量程DA值
#define TEMPDAMIN                         (CAPDAMAX      + 0x02)                ///< 温度零点DA值
#define TEMPDAMAX                         (TEMPDAMIN     + 0x02)                ///< 温度满量程DA值
#define CORRECT_K                         (TEMPDAMAX     + 0x02)                ///< 电容修正系数K
#define CORRECT_B                         (CORRECT_K     + 0x02)                ///< 电容修正系数B
#define TEMPER_K1                         (CORRECT_B     + 0x02)                ///< 温度1修正系数K1
#define TEMPER_B1                         (TEMPER_K1     + 0x02)                ///< 温度1修正系数B1
#define TEMPER_K2                         (TEMPER_B1     + 0x02)                ///< 温度2修正系数K2
#define TEMPER_B2                         (TEMPER_K2     + 0x02)                ///< 温度2修正系数B2
#define SYSTEMPARAM_PROGRAMED             (TEMPER_B2     + 0x02)                /** 用于检查Flash标定参数区
                                                                                是否被写入过初始值，值为
                                                                                0xAA则写入过 */
#define SYSTEM_PARAM_BAK1                 (RUN_ADDR_BASE     + FLASH_PAGE_SIZE) ///< 系统参数备份1起始地址
#define SYSTEM_PARAM_BAK2                 (SYSTEM_PARAM_BAK1 + FLASH_PAGE_SIZE) ///< 系统参数备份2起始地址     

#define ORGANIZATION                       0x0400                               ///< 组织机构代码
#define PRODUCTION                        (ORGANIZATION  + 0x30)                ///< 产品代码
#define HARDWAREVER                       (PRODUCTION    + 0x30)                ///< 硬件版本
#define SOFTWAREVER                       (HARDWAREVER   + 0x30)                ///< 软件版本
#define DEVICENUM                         (SOFTWAREVER   + 0x20)                ///< 设备ID
#define CUSTOMER                          (DEVICENUM     + 0x30)                ///< 客户代码


/***************************** 设备参数占用存储空间的长度 ****************************/                                                                      
#define USER_DEFAULT_LEN                  (OUTPUTMODE    + 0x01)                ///< 基本参数总长度   
#define PRO_DEFAULT_LEN                   (SYSTEMPARAM_PROGRAMED + 0x01)        ///< 全部参数总长度   


/********************************** Flash写入标志值 *********************************/   
#define SYSTEMPARAM_IS_PROGRAMED          0xAA                                  ///< 写入初始值标志



/**@brief       向STM32F072xx内部Flash指定位置写多个字节且备份3份
* @param[in]    FlashAddr : 写起始地址
* @param[in]    pWrbuf : 要写入的数据缓存指针;
* @param[in]    Wrlen : 数据长度
* @return       函数执行结果
* - OP_SUCCESS(成功)
* - OP_FAILED(失败)
*/
uint8_t InFlash_Write3T_MultiBytes(uint16_t FlashAddr, const uint8_t *pWrbuf, uint16_t Wrlen);

/**@brief       内部Flash系统参数检查,若出现不一致的参数,重新将User_Default_Param
* 写入内部Flash
* @param[in]    Cur_Param : 要检查的参数数组;
* @param[in]    Num_Of_Cur_Param : 参数数组的长度;
* @return       函数执行结果
* - None
*/
void InFlash_SystemParam_Check(uint8_t *Cur_Param, uint16_t Num_Of_Cur_Param);

/**@brief       读取并检查保存在内部Flash的设备参数
* @return       函数执行结果
* - None
*/
void Check_Device_Param(void);

#endif
