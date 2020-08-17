/**@file        task.h
* @details      task.c的头文件,声明了task应用的API函数
* @author       马灿林
* @date         2020-08-14
* @version      V1.0.0
* @copyright    2020-2030,深圳市信为科技发展有限公司
**********************************************************************************
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/08/14  <td>1.0.0    <td>马灿林    <td>创建初始版本
* </table>
*
**********************************************************************************
*/

#ifndef __TASK_H
#define __TASK_H

#include "main.h"
#include "board.h"
#include "rtthread.h"

uint8_t task_init(void);

#endif 
