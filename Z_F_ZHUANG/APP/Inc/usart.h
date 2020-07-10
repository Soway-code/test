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

extern uint8_t UART1_RXBuff[USART_MAX_DATALEN];//�������ݻ���
extern uint8_t UART1_TXBuff[USART_MAX_DATALEN];//�������ݻ���


#define EN_485_PORT             GPIOA
#define EN_485_PIN              GPIO_PIN_9
#define TX_ON                   HAL_GPIO_WritePin(EN_485_PORT, EN_485_PIN, GPIO_PIN_SET)
#define TX_OFF                  HAL_GPIO_WritePin(EN_485_PORT, EN_485_PIN, GPIO_PIN_RESET)

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

/**
* @brief      ���ڳ�ʼ��
* @retval     None
*/
void MX_USART1_UART_Init(void);
/**
* @brief       ����DMA����
* @param[in]    pData : DMA���ͻ���ָ��;
* @param[in]    Size : �����С
* @return       ����ִ�н��
* - success(�ɹ�)
* - fail(ʧ��)
*/
uint8_t UART_DMA_Send(uint8_t *pData, uint16_t Size);
/**
* @brief       ����DMA����
* @param[in]    pData : DMA���ջ���ָ��;
* @param[in]    Size : �����С
* @return       ����ִ�н��
* - success(�ɹ�)
* - fail(ʧ��)
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
