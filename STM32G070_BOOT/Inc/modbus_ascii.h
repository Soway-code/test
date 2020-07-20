#ifndef _MODBUS_ASCII_H
#define _MODBUS_ASCII_H
#include "stm32g0xx_hal.h"
#include "common.h"

#define ASCII_HEAD_STATE  0     //ͷ
#define ASCII_END_STATE   1     //����
#define ASCII_IDLE_STATE  2     //����

#define ASCII_HEAD_DATA   ':'           //��ͷ��һ���ֽ�

void MODBUS_ASCII_HexToAscii(uint8_t cyHexData, uint8_t *pCyAsciiBuf);
uint8_t MODBUS_ASCII_AsciiToHex(uint8_t *pCyAsciiBuf);
uint8_t MODBUS_ASCII_GetLrc(uint8_t *pCyAsciiBuf, uint16_t cyLen);
uint16_t MODBUS_ASCII_AsciiPacketToRtuPacket(uint8_t *pCyAsciiBuf, uint16_t cyAsciiLen, uint8_t *pCyRtuBuf);
uint16_t MODBUS_ASCII_RtuPacketToAsciiPacket(uint8_t *pCyRtuBuf, uint16_t cyRtuLen, uint8_t *pCyAsciiBuf);
void MODBUS_ASCII_HandlRevData(uint8_t cyRevData);
uint8_t MODBUS_ASCII_CheckAscii(uint8_t *pCyAsciiBuf, uint16_t cyLen);
uint8_t MODBUS_ASCII_RecvData(uint8_t* cyRecvBuff, uint16_t *pCyLen);
uint16_t MODBUS_ASCII_SendData(uint8_t *cySendBuff, uint16_t cyLen);
//uint16_t UART1_SendData(uint8_t* UART1_SendBuff, uint16_t Len);
//uint16_t UART1_RecvData(uint8_t* UART1_RecvBuff, uint16_t Len);

#endif

