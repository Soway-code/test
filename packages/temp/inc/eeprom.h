#ifndef _EEPROM_H
#define _EEPROM_H


#include "stm32l0xx.h"

#define EEPROM_BASE_ADDR        0x08080000    
#define EEPROM_BYTE_SIZE        0x17FF
#define EEPROM_TEMP_ADDR        0x1700 
#define EN_INT                  __enable_irq();     //ϵͳ��ȫ���ж�  
#define DIS_INT                 __disable_irq();    //ϵͳ��ȫ���ж�


//void Eeprom_WriteByte(uint16_t pWriteAddr, uint8_t pData);
//void Eeprom_ReadByte(uint16_t pReadAddr, uint8_t *pReadData);
void Eeprom_WriteNBytes(uint16_t pWriteAddr, uint8_t *pBuffer, uint16_t pLen);
void Eeprom_ReadNBytes(uint16_t pReadAddr, uint8_t *pBuffer, uint16_t pLen);
void Eeprom_WNBytesMul3T(uint16_t WriteAddr, uint8_t *pBuffer, uint16_t NumToWrite);
#endif
