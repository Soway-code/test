/**@file        spi.h
* @details      spi.c的头文件,定义了spi应用的宏定义,声明了spi初始化函数
* @author       马灿林
* @date         2020-07-25
* @version      V1.0.0
* @copyright    2020-2030,深圳市信为科技发展有限公司
**********************************************************************************
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/07/25  <td>1.0.0    <td>马灿林    <td>创建初始版本
* </table>
*
**********************************************************************************
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

/**
*@brief CPOL：时钟极性选择  
*CPOL：时钟极性选择,为SPI_POLARITY_LOW时SPI总线空闲为低电平，为SPI_POLARITY_HIGH时SPI总线空闲为高电平
*/
#define CPOL 	SPI_POLARITY_LOW	 
//#define CPOL	SPI_POLARITY_HIGH


/**
*@brief CPHA：时钟相位选择 
*CPHA：时钟相位选择，为SPI_PHASE_1EDGE时在SCK第一个跳变沿采样，为SPI_PHASE_2EDGE时在SCK第二个跳变沿采样
*/
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
