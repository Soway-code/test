/**@file        task.h
* @details      task.c��ͷ�ļ�,������taskӦ�õ�API����
* @author       �����
* @date         2020-07-17
* @version      V1.0.0
* @copyright    2020-2030,��������Ϊ�Ƽ���չ���޹�˾
**********************************************************************************
* @par �޸���־:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/07/17  <td>1.0.0    <td>�����    <td>������ʼ�汾
* </table>
*
**********************************************************************************
*/
#ifndef __TASK_H
#define __TASK_H

#include "board.h"
#include "rtthread.h"
#include "main.h"
#include "usart.h"
#include "oled.h"
#include "ds18b20.h"

/**@brief       ����ĳ�ʼ������
* @param[in]    ��
* @param[out]   ��
* @return       ��������ʼ�����
* - 1(�ɹ�)
* - 0(ʧ��)
*/
uint8_t task_init(void);

#endif 
