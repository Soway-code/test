/**@file        task.h
* @details      task.c的头文件,声明了task应用的API函数
* @author       马灿林
* @date         2020-08-25
* @version      V1.0.0
* @copyright    2020-2030,深圳市信为科技发展有限公司
**********************************************************************************
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/08/25  <td>1.0.0    <td>马灿林    <td>创建初始版本
* </table>
*
**********************************************************************************
*/

#ifndef __TASK_H__
#define __TASK_H__

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

/**@brief       任务的初始化函数
* @param[in]    无
* @param[out]   无
* @return       任务函数初始化结果
* - RT_EOK(成功)
* - RT_ERROR(失败)
*/
uint8_t task_init(void);

#endif 

