/**
  ******************************************************************************
  * File Name          : SPI.h
  * Description        : This file provides code for the configuration
  *                      of the SPI instances.
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __spi_H
#define __spi_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern SPI_HandleTypeDef hspi1;

/* USER CODE BEGIN Private defines */

	//PA1     ------> SPI1_SCK
    //PA2     ------> SPI1_MOSI
    //PA6     ------> SPI1_MISO 
#define SPI1_SCK_PIN 	GPIO_PIN_1
#define	SPI1_MOSI_PIN	GPIO_PIN_2	
#define SPI1_MISO_PIN	GPIO_PIN_6
#define SPI1_GPIOx		GPIOA

//CPOL：时钟极性选择,为0时SPI总线空闲为低电平，为1时SPI总线空闲为高电平
#define CPOL 	SPI_POLARITY_LOW	 
//#define CPOL	SPI_POLARITY_HIGH

//CPHA：时钟相位选择，为0时在SCK第一个跳变沿采样，为1时在SCK第二个跳变沿采样
#define CPHA	SPI_PHASE_1EDGE				
//#define CPHA	SPI_PHASE_2EDGE

/* USER CODE END Private defines */

void MX_SPI1_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ spi_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
