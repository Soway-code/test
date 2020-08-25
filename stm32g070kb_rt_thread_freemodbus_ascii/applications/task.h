/**@file        task.h
* @details      task.c��ͷ�ļ�,������taskӦ�õ�API����
* @author       �����
* @date         2020-08-25
* @version      V1.0.0
* @copyright    2020-2030,��������Ϊ�Ƽ���չ���޹�˾
**********************************************************************************
* @par �޸���־:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/08/25  <td>1.0.0    <td>�����    <td>������ʼ�汾
* </table>
*
**********************************************************************************
*/

#ifndef __TASK_H__
#define __TASK_H__

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

/**@brief       ����ĳ�ʼ������
* @param[in]    ��
* @param[out]   ��
* @return       ��������ʼ�����
* - RT_EOK(�ɹ�)
* - RT_ERROR(ʧ��)
*/
uint8_t task_init(void);

#endif 

