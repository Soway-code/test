/**@file        iwdg_bsp.c
* @brief        独立看门狗驱动
* @details      初始化独立看门狗,在系统出现异常时复位
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

#include "iwdg_bsp.h"


static IWDG_HandleTypeDef hiwdg;        ///< 看门狗处理对象


/**@brief       独立看门狗初始化
* @return       函数执行结果
* - None
* @note         看门狗时钟由内部低速振荡器LSI分频后得到，溢出时间 = 分频系数 * 
* 重载值 / LSI频率 s，例如8分频，重载值 = 4000，LSI频率 = 40KHz，则溢出时间为
* 8 * 4000 / 40000 = 0.8 s
*/
void BSP_IWDG_Init(void)
{

  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
  hiwdg.Init.Window = IWDG_WINDOW_DISABLE;
  hiwdg.Init.Reload = IWDG_TIMEOUT * (LSI_VALUE / 1000) / 32;   //1秒超时
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }

}

/**@brief       独立看门狗喂狗
* @return       函数执行结果
* - None
*/
void BSP_IWDG_Refresh(void)
{
    HAL_IWDG_Refresh(&hiwdg);
}

