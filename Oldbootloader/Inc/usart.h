#ifndef _USART1_H
#define _USART1_H

#include "stm32g0xx_hal.h"
#include "stm32g0xx_ll_system.h"

#define EN_485_PORT             GPIOA
#define EN_485_PIN              GPIO_PIN_9
#define TX_ON                   HAL_GPIO_WritePin(EN_485_PORT, EN_485_PIN, GPIO_PIN_SET)
#define TX_OFF                  HAL_GPIO_WritePin(EN_485_PORT, EN_485_PIN, GPIO_PIN_RESET)
#define USART1_MAX_DATALEN      600    

extern UART_HandleTypeDef huart1; 
void UART_DeInit(void);
void Uart_Config_Init(void);
int32_t UART1_RecvData(uint8_t *UART1_RecvBuff, uint32_t Len);
uint32_t UART1_SendData(uint8_t *UART1_SendBuff, uint32_t Len);
void MX_LPUART1_UART_Init(void);
void uprintf(const char *fmt,...);

#endif

