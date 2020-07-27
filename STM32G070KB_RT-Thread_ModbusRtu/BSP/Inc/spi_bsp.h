/**@file        spi_bsp.h
* @details      spi_bsp.c的头文件,声明了硬件SPI的API函数
* @author       杨春林
* @date         2020-05-20
* @version      V1.0.0
* @copyright    2020-2030,深圳市信为科技发展有限公司
**********************************************************************************
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/05/20  <td>1.0.0    <td>杨春林    <td>创建初始版本
* </table>
*
**********************************************************************************
*/

#ifndef __SPI_BSP_H
#define __SPI_BSP_H
#ifdef __cplusplus
 extern "C" {
#endif


#include "main.h"
#include "VariaType.h"

#define SPIx                            SPI1
#define SPIx_CLK_ENABLE()               __HAL_RCC_SPI1_CLK_ENABLE()
#define SPIx_SCK_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_MISO_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_MOSI_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()

#define SPIx_CLK_DISABLE()              __HAL_RCC_SPI1_CLK_DISABLE()

/* Definition for SPIx Pins */
#define SPIx_SCK_PIN                    GPIO_PIN_3
#define SPIx_SCK_GPIO_PORT              GPIOB
#define SPIx_SCK_AF                     GPIO_AF0_SPI1
#define SPIx_MISO_PIN                   GPIO_PIN_4
#define SPIx_MISO_GPIO_PORT             GPIOB
#define SPIx_MISO_AF                    GPIO_AF0_SPI1
#define SPIx_MOSI_PIN                   GPIO_PIN_5
#define SPIx_MOSI_GPIO_PORT             GPIOB
#define SPIx_MOSI_AF                    GPIO_AF0_SPI1

#define USER_SPI_TIMEOUT                3000


/**@brief       SPI初始化
* @return       函数执行结果
* - None
*/
void BSP_SPI_Init(void);

/**@brief       SPI发送数据
* @param[in]    pData : 发送缓存指针;
* @param[in]    Size : 缓存大小
* @return       函数执行结果
* - OP_SUCCESS(成功)
* - OP_FAILED(失败)
*/
uint8_t BSP_SPI_Transmit(uint8_t *pData, uint16_t Size);

/**@brief       SPI接收数据
* @param[in]    pData : 接收缓存指针;
* @param[in]    Size : 缓存大小
* @return       函数执行结果
* - OP_SUCCESS(成功)
* - OP_FAILED(失败)
*/
uint8_t BSP_SPI_Receive(uint8_t *pData, uint16_t Size);

/**@brief       SPI发送并接收数据
* @param[in]    TxData : 发送缓存指针;
* @param[out]   RxData : 接收缓存指针;
* @param[in]    Size : 发送和接收缓存的大小
* @return       函数执行结果
* - OP_SUCCESS(成功)
* - OP_FAILED(失败)
*/
uint8_t BSP_SPI_TransmitReceive(uint8_t *TxData, uint8_t *RxData, uint16_t Size);


#ifdef __cplusplus
}
#endif
#endif 
