/**@file        iwdg_bsp.h
* @details      iwdg_bsp.c��ͷ�ļ�,�����˿��Ź�������API����
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

#ifndef __iwdg_H
#define __iwdg_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define IWDG_TIMEOUT        1000        ///< ��ʱʱ��,��λ����


/**@brief       �������Ź���ʼ��
* @return       ����ִ�н��
* - None
* @note         ���Ź�ʱ�����ڲ���������LSI��Ƶ��õ������ʱ�� = ��Ƶϵ�� * 
* ����ֵ / LSIƵ�� s������8��Ƶ������ֵ = 4000��LSIƵ�� = 40KHz�������ʱ��Ϊ
* 8 * 4000 / 40000 = 0.8 s
*/
void BSP_IWDG_Init(void);

/**@brief       �������Ź�ι��
* @return       ����ִ�н��
* - None
*/
void BSP_IWDG_Refresh(void);

#ifdef __cplusplus
}
#endif
#endif /*__ iwdg_H */

