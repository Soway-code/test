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
	uint8_t UpgradeFlag;														// 系统升级标志 存储物理地址 Addr_0x08003000 
	uint8_t Addr;																    // 从机ID 		存储物理地址 Addr_0x08003001
	uint8_t Baud;																    // 波特率		存储物理地址 Addr_0x08003002
	uint8_t Parity;																  // 奇偶校验 	存储物理地址 Addr_0x08003003
	uint8_t OutPutMod;															// 输出模式 	存储物理地址 Addr_0x08003004
	uint8_t ProgEraseFlag;
	uint8_t Reserved[10];														// 保留 留做备用 方便添加更多参数 按16字节对齐
}CommonPara_Typedef;															// 通用参数结构体 通用参数与客户可操作参数结构体部分参数重叠 仅存储格式不同
																				          // 目的是为了兼容BootLoad和Modbus等定义
typedef struct
{
  uint16_t Flag;																  // Flash写标志
  uint16_t TempH;                             	  // Addr_0x02 数据高16位 
  uint16_t TempL;                             	  // Addr_0x03 数据低16位
  uint16_t Addr;                             		  // Addr_0x30 设备地址
	uint16_t Baud;                             		  // Addr_0x31 波特率
	uint16_t Parity;                            	  // Addr_0x32 奇偶校验
	uint16_t Compensation;                      	  // Addr_0x34 补偿使能
	uint16_t UploadFreq;                      		  // Addr_0x3F 自动上传时间
  uint16_t UpgradeFlag;								// Addr_0x40 系统升级标志
  uint16_t Reset;                             	  // Addr_0x51 恢复出厂设置
  uint16_t OutPutMod;                         	  // Addr_0x60 输出模式
	uint16_t Reserved[5];													  // 保留 留做备用 方便添加更多参数 按16字对齐
}CustomerPara_Typedef;	

typedef struct
{
	CommonPara_Typedef		Common;												// 通用参数结构体 通用参数与客户可操作参数结构体部分参数重叠 仅存储格式不同 为兼容BootLoad和Modbus规范 
	CustomerPara_Typedef	Customer;											// 客户可操作参数结构体 

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
