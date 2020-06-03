#ifndef _MODBUS_ASCII_H
#define _MODBUS_ASCII_H
#include "stm32L0xx.h"

#define ASCII_HEAD_STATE  0     //头
#define ASCII_END_STATE   1     //结束
#define ASCII_IDLE_STATE  2     //空闲

#define ASCII_HEAD_DATA   ':'           //包头第一个字节

void MODBUS_ASCII_HexToAscii(uint8_t cyHexData, uint8_t *pCyAsciiBuf);
uint8_t MODBUS_ASCII_AsciiToHex(uint8_t *pCyAsciiBuf);
uint8_t MODBUS_ASCII_GetLrc(uint8_t *pCyAsciiBuf, uint8_t cyLen);
uint8_t MODBUS_ASCII_AsciiPacketToRtuPacket(uint8_t *pCyAsciiBuf, uint8_t cyAsciiLen, uint8_t *pCyRtuBuf);
uint8_t MODBUS_ASCII_RtuPacketToAsciiPacket(uint8_t *pCyRtuBuf, uint8_t cyRtuLen, uint8_t *pCyAsciiBuf);
void MODBUS_ASCII_HandlRevData(uint8_t cyRevData);
void MODBUS_ASCII_HandlRevData_C(uint8_t cyRevData);
uint8_t MODBUS_ASCII_CheckAscii(uint8_t *pCyAsciiBuf, uint8_t cyLen);
uint8_t MODBUS_ASCII_RecvData(uint8_t* cyRecvBuff, uint8_t *pCyLen);
uint8_t MODBUS_ASCII_SendData(uint8_t *cySendBuff, uint8_t cyLen);

#endif
