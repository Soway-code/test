/**@file        spi_bsp.c
* @brief        硬件SPI驱动
* @details      硬件SPI初始化, 为SPI数据收发提供API函数
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

#include "spi_bsp.h"

/* 使用RT-Thread操作系统,USING_RT_THREAD_OS在main.h中定义 */
#ifdef USING_RT_THREAD_OS
#include "board.h"
#endif

static SPI_HandleTypeDef hspi1;     ///< SPI处理对象


/**@brief       SPI初始化
* @return       函数执行结果
* - None
*/
void BSP_SPI_Init(void)
{

  hspi1.Instance = SPIx;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }

}

/**@brief       SPI时钟、底层管脚初始化(由HAL_ADC_Init回调调用)
* @param[in]    spiHandle, SPI处理对象
* @return       函数执行结果
* - None
*/
void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    SPIx_CLK_ENABLE();
  
    SPIx_SCK_GPIO_CLK_ENABLE();
    SPIx_MISO_GPIO_CLK_ENABLE();
    SPIx_MOSI_GPIO_CLK_ENABLE();

    GPIO_InitStruct.Pin = SPIx_SCK_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = SPIx_SCK_AF;
    HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = SPIx_MISO_PIN;
    GPIO_InitStruct.Alternate = SPIx_MISO_AF;
    HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = SPIx_MOSI_PIN;
    GPIO_InitStruct.Alternate = SPIx_MOSI_AF;
    HAL_GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStruct);
}

/**@brief       SPI时钟、底层管脚反初始化(由HAL_ADC_DeInit回调调用)
* @param[in]    spiHandle, SPI处理对象
* @return       函数执行结果
* - None
*/
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{
    SPIx_CLK_DISABLE();
     
    HAL_GPIO_DeInit(SPIx_SCK_GPIO_PORT, SPIx_SCK_PIN);
    HAL_GPIO_DeInit(SPIx_MISO_GPIO_PORT, SPIx_MISO_PIN);
    HAL_GPIO_DeInit(SPIx_MOSI_GPIO_PORT, SPIx_MOSI_PIN);
} 

/**@brief       SPI发送数据
* @param[in]    pData : 发送缓存指针;
* @param[in]    Size : 缓存大小
* @return       函数执行结果
* - OP_SUCCESS(成功)
* - OP_FAILED(失败)
*/
uint8_t BSP_SPI_Transmit(uint8_t *pData, uint16_t Size)
{
    if(HAL_SPI_Transmit(&hspi1, pData, Size, USER_SPI_TIMEOUT) != HAL_OK)
    {
        return OP_FAILED;
    }
    return OP_SUCCESS;
}

/**@brief       SPI接收数据
* @param[in]    pData : 接收缓存指针;
* @param[in]    Size : 缓存大小
* @return       函数执行结果
* - OP_SUCCESS(成功)
* - OP_FAILED(失败)
*/
uint8_t BSP_SPI_Receive(uint8_t *pData, uint16_t Size)
{
    if(HAL_SPI_Receive(&hspi1, pData, Size, USER_SPI_TIMEOUT) != HAL_OK)
    {
        return OP_FAILED;
    }
    return OP_SUCCESS;
}

/**@brief       SPI发送并接收数据
* @param[in]    TxData : 发送缓存指针;
* @param[out]   RxData : 接收缓存指针;
* @param[in]    Size : 发送和接收缓存的大小
* @return       函数执行结果
* - OP_SUCCESS(成功)
* - OP_FAILED(失败)
*/
uint8_t BSP_SPI_TransmitReceive(uint8_t *TxData, uint8_t *RxData, uint16_t Size)
{
    if(HAL_SPI_TransmitReceive(&hspi1, TxData, RxData, Size, USER_SPI_TIMEOUT) != HAL_OK)
    {
        return OP_FAILED;
    }
    return OP_SUCCESS;
}

