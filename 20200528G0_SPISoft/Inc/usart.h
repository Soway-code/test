#ifndef _USART_H
#define _USART_H
#include "main.h"
#define DataPacketLen				200											// 串口接收缓存区长度

#define RS485_TX_EN		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET)  	// 发送使能						
#define RS485_RX_EN		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET)   // 接收使能
extern uint8_t RecBuf[DataPacketLen]; 
extern UART_HandleTypeDef hlpuart1;
void MX_LPUART1_UART_Init(void);

void USART1_RecMod_RTU(void);													// 串口接收RTU模式
void USART1_RecMod_ASCII(void);												// 串口接收ASCII模式
void MDB_Get_master_request_data(uint8_t *destination, uint8_t *source, uint8_t len);	// 获取主机查询帧数据

#endif

