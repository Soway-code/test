/**@file        iwdg_bsp.h
* @details      iwdg_bsp.c的头文件,声明了看门狗驱动的API函数
* @author       杨春林
* @date         2020-04-27
* @version      V1.0.0
* @copyright    2020-2030,深圳市信为科技发展有限公司
**********************************************************************************
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/04/27  <td>1.0.0    <td>杨春林    <td>创建初始版本
* </table>
*
**********************************************************************************
*/

#ifndef __iwdg_H
#define __iwdg_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define IWDG_TIMEOUT        1000        ///< 超时时间,单位毫秒


/**@brief       独立看门狗初始化
* @return       函数执行结果
* - None
* @note         看门狗时钟由内部低速振荡器LSI分频后得到，溢出时间 = 分频系数 * 
* 重载值 / LSI频率 s，例如8分频，重载值 = 4000，LSI频率 = 40KHz，则溢出时间为
* 8 * 4000 / 40000 = 0.8 s
*/
void BSP_IWDG_Init(void);

/**@brief       独立看门狗喂狗
* @return       函数执行结果
* - None
*/
void BSP_IWDG_Refresh(void);

#ifdef __cplusplus
}
#endif
#endif /*__ iwdg_H */

