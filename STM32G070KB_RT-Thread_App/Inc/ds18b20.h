/**@file        ds18b20.h
* @details      ds18b20.c的头文件,定义了配置ds18b20管脚和定时器应用的宏定义,声明了ds18b20应用的API函数
* @author       马灿林
* @date         2020-07-17
* @version      V1.0.0
* @copyright    2020-2030,深圳市信为科技发展有限公司
**********************************************************************************
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/07/17  <td>1.0.0    <td>马灿林    <td>创建初始版本
* </table>
*
**********************************************************************************
*/

#ifndef __DS18B20_H
#define __DS18B20_H
#include "main.h"
#include "board.h"
#include "rtthread.h"

/**
*@brief RCC_GPIOX_CLK 
*RCC_GPIOX_CLK 		//开启GPIOx时钟
*DS18B20_GPIOX 		//选择GPIOx端口
*DS18B20_GPIO_PINX	//选择管脚
*/
#define RCC_GPIOX_CLK 		__HAL_RCC_GPIOA_CLK_ENABLE();						///开启GPIOB时钟
#define DS18B20_GPIOX 		GPIOA
#define DS18B20_GPIO_PINX 	GPIO_PIN_15

////IO操作函数											   
#define	DS18B20_DQ_IN  HAL_GPIO_ReadPin(DS18B20_GPIOX,DS18B20_GPIO_PINX)  //数据端口	PA15
 
/**
* @brief uint8_t DS18B20_Init(void);
*
* @return 1,0 
* @retval 返回1 DS18B20不存在, 返回0 DS18B20存在初始化成功
*/
uint8_t DS18B20_Init(void);			

/**
* @brief short DS18B20_Get_Temp(void);
*
* @return 返回值：获取到的温度温度值(-550~1250)
* @retval 返回值是温度的十倍 (24.5℃时返回245)	
*/
short DS18B20_Get_Temp(void);			

/**
* @brief 开始温度转换
*/
void DS18B20_Start(void);		

/**
* @brief 写入一个字节
*/
void DS18B20_Write_Byte(uint8_t dat);

/**
* @brief 读出一个字节
*/
uint8_t DS18B20_Read_Byte(void);

/**
* @brief 读出一个位
*/
uint8_t DS18B20_Read_Bit(void);	

/**
* @brief 检测是否存在DS18B20
*/
uint8_t DS18B20_Check(void);

/**
* @brief 复位DS18B20
*/
void DS18B20_Rst(void);		

#endif

