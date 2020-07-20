/**@file        modbus_ascii.h
* @details      modbus_ascii.c的头文件,声明了ModBus ASCII数据处理的API函数
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

#ifndef _MODBUS_ASCII_H
#define _MODBUS_ASCII_H
#ifdef __cplusplus
 extern "C" {
#endif
     
#include "stm32g0xx.h"
#include "common.h"


#define CMD_STRING_SIZE       128
#define USART_ASCII_MAX_DATALEN     600
#define USART_NOASC_MAX_DATALEN     300
#define USART_NOMAL_MAX_DATALEN     100
#define USART_ASCII_MIN_DATALEN     80

#define ASCII_HEAD_STATE  0     //头
#define ASCII_END_STATE   1     //结束
#define ASCII_IDLE_STATE  2     //空闲

#define ERR_NONE            0
#define ERR_ASCII           1
#define ERR_LRC             2
#define ERR_INVALID_DATA    3

#define ASCII_HEAD_DATA         ':'         //包头第一个字节
#define ASCII_TAIL_FIRST_DATA   '\r'        //包尾第一个字节
#define ASCII_TAIL_SECOND_DATA  '\n'        //包尾第二个字节

/**@brief       十六进制数转ASCII码
* @param[in]    cyHexData : 十六进制数
* @param[out]   pCyAsciiBuf : ASCII码
* @return       函数执行结果
* - None
*/
void MODBUS_ASCII_HexToAscii(uint8_t cyHexData, uint8_t *pCyAsciiBuf);

/**@brief       ASCII码转十六进制数
* @param[in]    pCyAsciiBuf : ASCII码
* @return       函数执行结果
* - 十六进制数
*/
uint8_t MODBUS_ASCII_AsciiToHex(uint8_t *pCyAsciiBuf);

/**@brief       获取LRC值
* @param[in]    RtuBuf : RTU数据缓存指针
* @param[in]    cyLen : 数据长度
* @return       函数执行结果
* - LRC值
*/
uint8_t MODBUS_GetLrc(uint8_t *pCyRtuBuf, uint16_t cyLen);

/**@brief       ASCII数据包转成RTU数据包
* @param[in]    pCyAsciiBuf : ASCII数据包指针
* @param[in]    cyAsciiLen : ASCII数据包长度
* @param[out]   pCyRtuBuf : RTU数据包指针
* @return       函数执行结果
* - 错误 : 0
* - 成功 : RTU数据包长度
*/
uint16_t MODBUS_ASCII_AsciiPacketToRtuPacket(uint8_t *pCyAsciiBuf, uint16_t cyAsciiLen, uint8_t *pCyRtuBuf);

/**@brief       RTU数据包转成ASCII数据包
* @param[in]    pCyRtuBuf : RTU数据包指针
* @param[in]    cyRtuLen : RTU数据包长度
* @param[out]   pCyAsciiBuf : ASCII数据包指针
* @return       函数执行结果
* - ASCII数据包长度
*/
uint16_t MODBUS_ASCII_RtuPacketToAsciiPacket(uint8_t *pCyRtuBuf, uint16_t cyRtuLen, uint8_t *pCyAsciiBuf);

/**@brief       检验是否都是Ascii码
* @param[in]    pCyAsciiBuf : ASCII数据包指针
* @param[in]    cyLen : ASCII数据包长度
* @return       函数执行结果
* - ERR_NONE : 无错误
* - ERR_ASCII : 不全是ASCII码
*/
uint8_t MODBUS_ASCII_CheckAscii(uint8_t *pCyAsciiBuf, uint16_t cyLen);

/**@brief       获取一包数据
* @param[in]    cyRecvBuff : 接收缓存指针
* @param[in]    cyLen : 接收的数据长度
* @return       函数执行结果
* - ERR_INVALID_DATA : 接收缓存指针无效
* - ERR_ASCII : 不全是ASCII码
* - ERR_LRC : LRC校验错误
* - ERR_NONE : 无错误
*/
uint8_t MODBUS_ASCII_RecvData(uint8_t* cyRecvBuff, uint16_t *pCyLen);

/**@brief       发送一包数据
* @param[in]    cySendBuff : 发送缓存指针
* @param[in]    cyLen : 发送的数据长度
* @return       函数执行结果
* - OP_SUCCESS(成功)
* - OP_FAILED(失败)
*/
uint8_t MODBUS_ASCII_SendData(uint8_t *cySendBuff, uint16_t cyLen);

#ifdef __cplusplus
}
#endif
#endif

