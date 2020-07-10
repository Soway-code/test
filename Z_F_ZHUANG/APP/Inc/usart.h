/**
  ******************************************************************************
  * File Name          : USART.h
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __usart_H
#define __usart_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

//typedef enum
//{ 
//  Bit_RESET = 0,
//  Bit_SET = 1
//}BitAction;

#define USART_MAX_DATALEN       150  
#define RXBUFFERSIZE            1

#define  success                0x00
#define  fail                   0x01

extern uint8_t rx_flag;
extern uint16_t rx_len;

extern UART_HandleTypeDef huart1;

extern uint8_t UART1_RXBuff[USART_MAX_DATALEN];//接收数据缓存
extern uint8_t UART1_TXBuff[USART_MAX_DATALEN];//发送数据缓存


#define EN_485_PORT             GPIOA
#define EN_485_PIN              GPIO_PIN_9
#define TX_ON                   HAL_GPIO_WritePin(EN_485_PORT, EN_485_PIN, GPIO_PIN_SET)
#define TX_OFF                  HAL_GPIO_WritePin(EN_485_PORT, EN_485_PIN, GPIO_PIN_RESET)

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/**
* @brief      串口初始化
* @retval     None
*/
void MX_USART1_UART_Init(void);
/**
* @brief       串口DMA发送
* @param[in]    pData : DMA发送缓存指针;
* @param[in]    Size : 缓存大小
* @return       函数执行结果
* - success(成功)
* - fail(失败)
*/
uint8_t UART_DMA_Send(uint8_t *pData, uint16_t Size);
/**
* @brief       串口DMA接收
* @param[in]    pData : DMA接收缓存指针;
* @param[in]    Size : 缓存大小
* @return       函数执行结果
* - success(成功)
* - fail(失败)
*/
uint8_t UART_DMA_Recv(uint8_t *pData, uint16_t Size);


/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
