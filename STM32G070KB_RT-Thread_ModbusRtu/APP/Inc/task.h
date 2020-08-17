/**@file        task.h
* @details      task.c的头文件,声明了task应用的API函数
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
#ifndef __TASK_H
#define __TASK_H

#include "board.h"
#include "rtthread.h"
#include "main.h"
#include "oled.h"
#include "ds18b20.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "In_Flash_app.h"
#include "ModBusRtu.h"
#include "stdio.h"
#include "tim.h"

/**@brief       任务的初始化函数
* @param[in]    无
* @param[out]   无
* @return       任务函数初始化结果
* - RT_EOK(0) (成功)
* - RT_ERROR(1)(失败)
*/
uint8_t task_init(void);

#endif 
