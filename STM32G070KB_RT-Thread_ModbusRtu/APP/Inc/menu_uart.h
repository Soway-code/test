/**@file        menu_uart.h
* @details      menu_uart.c的头文件,声明了串口数据包处理的API函数
* @author       杨春林
* @date         2020-06-04
* @version      V1.0.0
* @copyright    2020-2030,深圳市信为科技发展有限公司
**********************************************************************************
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/06/04  <td>1.0.0    <td>杨春林    <td>创建初始版本
* </table>
*
**********************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MENU_UART_H
#define __MENU_UART_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "flash_if.h"
//#include "AT24cxx.h"
//#include "usart.h"
#include "modbus_ascii.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#define RESPONSE_ERR_NONE   0     //响应成功
#define RESPONSE_REC_ERR    1     //接收错误
#define RESPONSE_LRC_ERR    2     //校验码错误

#define ADDR_DEVICEADDR     0   
#define ADDR_ERASEFLAG      2046
#define ADDR_UPGRADEFLAG    2047

#define UPGRADED_DEVICEADDR     65

#define ERASE_FLAG          0x0C
#define ERASE_FLAG_NONE     0xFF

#define UPGRADE_FLAG        0x0C
#define UPGRADE_FLAG_NONE   0xFF

/** ModBus处理的基本参数结构 */
typedef struct {
    uint8_t Device_Addr;        //设备地址
    uint8_t ProgErase;          //程序擦除标志
    int8_t UpgradeWaitTime;    //升级等待时间
    uint8_t *RX_buf;            //接收缓存
}ModBus_Param_TypeDef;


/**@brief       向存储器写入 1 字节数据
* @param[in]    addr : 写入数据的地址
* @param[in]    WrData : 写入的数据
* @return       函数执行结果
* - OP_SUCCESS(成功)
* - OP_FAILED(失败)
* @note         本函数需要自行添加写入存储器的API函数
*/
uint8_t Write_Memory_1_Byte(uint16_t addr, uint8_t WrData);

/**@brief       向存储器读取 1 字节数据
* @param[in]    addr : 读取地址
* @return       函数执行结果
* - 1 字节数据
* @note         本函数需要自行添加读取存储器的API函数
*/
uint8_t Read_Memory_1_Byte(uint16_t addr);

/**@brief       向存储器写入 1 字节数据
* @param[in]    addr : 写入地址
* @param[in]    WrData : 写入的数据
* @return       函数执行结果
* - OP_SUCCESS(成功)
* - OP_FAILED(失败)
* @note         本函数需要自行添加写入存储器的API函数,
* 这里添加的是写芯片内部Flash函数
*/
void HandleFrameInformation(ModBus_Param_TypeDef *ModBus_Param);

#ifdef __cplusplus
}
#endif
#endif

