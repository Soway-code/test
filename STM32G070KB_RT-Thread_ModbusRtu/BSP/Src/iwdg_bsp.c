/**@file        iwdg_bsp.c
* @brief        �������Ź�����
* @details      ��ʼ���������Ź�,��ϵͳ�����쳣ʱ��λ
* @author       ���
* @date         2020-04-27
* @version      V1.0.0
* @copyright    2020-2030,��������Ϊ�Ƽ���չ���޹�˾
**********************************************************************************
* @par �޸���־:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/04/27  <td>1.0.0    <td>���    <td>������ʼ�汾
* </table>
*
**********************************************************************************
*/

#include "iwdg_bsp.h"


static IWDG_HandleTypeDef hiwdg;        ///< ���Ź��������


/**@brief       �������Ź���ʼ��
* @return       ����ִ�н��
* - None
* @note         ���Ź�ʱ�����ڲ���������LSI��Ƶ��õ������ʱ�� = ��Ƶϵ�� * 
* ����ֵ / LSIƵ�� s������8��Ƶ������ֵ = 4000��LSIƵ�� = 40KHz�������ʱ��Ϊ
* 8 * 4000 / 40000 = 0.8 s
*/
void BSP_IWDG_Init(void)
{

  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
  hiwdg.Init.Window = IWDG_WINDOW_DISABLE;
  hiwdg.Init.Reload = IWDG_TIMEOUT * (LSI_VALUE / 1000) / 32;   //1�볬ʱ
  if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
  {
    Error_Handler();
  }

}

/**@brief       �������Ź�ι��
* @return       ����ִ�н��
* - None
*/
void BSP_IWDG_Refresh(void)
{
    HAL_IWDG_Refresh(&hiwdg);
}

