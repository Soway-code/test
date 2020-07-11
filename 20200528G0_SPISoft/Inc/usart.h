#ifndef _USART_H
#define _USART_H
#include "main.h"
#define DataPacketLen				200											// ���ڽ��ջ���������

#define RS485_TX_EN		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET)  	// ����ʹ��						
#define RS485_RX_EN		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET)   // ����ʹ��
extern uint8_t RecBuf[DataPacketLen]; 
extern UART_HandleTypeDef hlpuart1;
void MX_LPUART1_UART_Init(void);

void USART1_RecMod_RTU(void);													// ���ڽ���RTUģʽ
void USART1_RecMod_ASCII(void);												// ���ڽ���ASCIIģʽ
void MDB_Get_master_request_data(uint8_t *destination, uint8_t *source, uint8_t len);	// ��ȡ������ѯ֡����

#endif

