/**@file        spi.h
* @details      spi.c��ͷ�ļ�,������spiӦ�õĺ궨��,������spi��ʼ������
* @author       �����
* @date         2020-07-25
* @version      V1.0.0
* @copyright    2020-2030,��������Ϊ�Ƽ���չ���޹�˾
**********************************************************************************
* @par �޸���־:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/07/25  <td>1.0.0    <td>�����    <td>������ʼ�汾
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
*@brief CPOL��ʱ�Ӽ���ѡ��  
*CPOL��ʱ�Ӽ���ѡ��,ΪSPI_POLARITY_LOWʱSPI���߿���Ϊ�͵�ƽ��ΪSPI_POLARITY_HIGHʱSPI���߿���Ϊ�ߵ�ƽ
*/
#define CPOL 	SPI_POLARITY_LOW	 
//#define CPOL	SPI_POLARITY_HIGH


/**
*@brief CPHA��ʱ����λѡ�� 
*CPHA��ʱ����λѡ��ΪSPI_PHASE_1EDGEʱ��SCK��һ�������ز�����ΪSPI_PHASE_2EDGEʱ��SCK�ڶ��������ز���
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
