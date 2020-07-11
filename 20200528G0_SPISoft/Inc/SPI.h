
#ifndef _SPI_H
#define _SPI_H

#include "main.h"

#define SPI_MOSI_SET				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET)
#define SPI_MOSI_CLR				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET)

#define SPI_SCK_SET					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET)
#define SPI_SCK_CLR					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET)

#define SPI_MISO_STA                HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7)

void Delay_us(uint32_t us);
void SPI_Send_8Bit(uint8_t sBit8);
uint8_t SPI_Receive_8Bit(void);
void SPI_Bit_Set(void);
void SPI_Bit_Clr(void);
void SPI_WriteByte(uint8_t TxData);
uint8_t SPI_ReadByte(void);
uint8_t SPI_RW(uint8_t data);
uint8_t SOFT_SPI_RW(uint8_t byte);
void Soft_SPI_Write(uint8_t a);
uint8_t Soft_SPI_Read(void);
uint8_t Spi_SendByte(uint8_t dat);
uint8_t SPI_SendByte(uint8_t dt);
#endif
