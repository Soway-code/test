/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g0xx_hal.h"
#include "stm32g0xx_ll_system.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
typedef  void (*pFunction)(void);
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

typedef struct
{
	uint8_t UpgradeFlag;														// ϵͳ������־ �洢�����ַ Addr_0x08003000 
	uint8_t Addr;																    // �ӻ�ID 		�洢�����ַ Addr_0x08003001
	uint8_t Baud;																    // ������		�洢�����ַ Addr_0x08003002
	uint8_t Parity;																  // ��żУ�� 	�洢�����ַ Addr_0x08003003
	uint8_t OutPutMod;															// ���ģʽ 	�洢�����ַ Addr_0x08003004
	uint8_t ProgEraseFlag;
	uint8_t Reserved[10];														// ���� �������� ������Ӹ������ ��16�ֽڶ���
}CommonPara_Typedef;															// ͨ�ò����ṹ�� ͨ�ò�����ͻ��ɲ��������ṹ�岿�ֲ����ص� ���洢��ʽ��ͬ
																				          // Ŀ����Ϊ�˼���BootLoad��Modbus�ȶ���
typedef struct
{
  uint16_t Flag;																  // Flashд��־
  uint16_t TempH;                             	  // Addr_0x02 ���ݸ�16λ 
  uint16_t TempL;                             	  // Addr_0x03 ���ݵ�16λ
  uint16_t Addr;                             		  // Addr_0x30 �豸��ַ
	uint16_t Baud;                             		  // Addr_0x31 ������
	uint16_t Parity;                            	  // Addr_0x32 ��żУ��
	uint16_t Compensation;                      	  // Addr_0x34 ����ʹ��
	uint16_t UploadFreq;                      		  // Addr_0x3F �Զ��ϴ�ʱ��
  uint16_t UpgradeFlag;								// Addr_0x40 ϵͳ������־
  uint16_t Reset;                             	  // Addr_0x51 �ָ���������
  uint16_t OutPutMod;                         	  // Addr_0x60 ���ģʽ
	uint16_t Reserved[5];													  // ���� �������� ������Ӹ������ ��16�ֶ���
}CustomerPara_Typedef;	

typedef struct
{
	CommonPara_Typedef		Common;												// ͨ�ò����ṹ�� ͨ�ò�����ͻ��ɲ��������ṹ�岿�ֲ����ص� ���洢��ʽ��ͬ Ϊ����BootLoad��Modbus�淶 
	CustomerPara_Typedef	Customer;											// �ͻ��ɲ��������ṹ�� 

}FlashPara_Typedef;		
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED1_Pin GPIO_PIN_4
#define LED1_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
