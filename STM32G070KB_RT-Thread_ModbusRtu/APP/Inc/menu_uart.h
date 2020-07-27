/**@file        menu_uart.h
* @details      menu_uart.c��ͷ�ļ�,�����˴������ݰ������API����
* @author       ���
* @date         2020-06-04
* @version      V1.0.0
* @copyright    2020-2030,��������Ϊ�Ƽ���չ���޹�˾
**********************************************************************************
* @par �޸���־:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/06/04  <td>1.0.0    <td>���    <td>������ʼ�汾
* </table>
*
**********************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MENU_UART_H
#define __MENU_UART_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "flash_if.h"
//#include "AT24cxx.h"
//#include "usart.h"
#include "modbus_ascii.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#define RESPONSE_ERR_NONE   0     //��Ӧ�ɹ�
#define RESPONSE_REC_ERR    1     //���մ���
#define RESPONSE_LRC_ERR    2     //У�������

#define ADDR_DEVICEADDR     0   
#define ADDR_ERASEFLAG      2046
#define ADDR_UPGRADEFLAG    2047

#define UPGRADED_DEVICEADDR     65

#define ERASE_FLAG          0x0C
#define ERASE_FLAG_NONE     0xFF

#define UPGRADE_FLAG        0x0C
#define UPGRADE_FLAG_NONE   0xFF

/** ModBus����Ļ��������ṹ */
typedef struct {
    uint8_t Device_Addr;        //�豸��ַ
    uint8_t ProgErase;          //���������־
    int8_t UpgradeWaitTime;    //�����ȴ�ʱ��
    uint8_t *RX_buf;            //���ջ���
}ModBus_Param_TypeDef;


/**@brief       ��洢��д�� 1 �ֽ�����
* @param[in]    addr : д�����ݵĵ�ַ
* @param[in]    WrData : д�������
* @return       ����ִ�н��
* - OP_SUCCESS(�ɹ�)
* - OP_FAILED(ʧ��)
* @note         ��������Ҫ�������д��洢����API����
*/
uint8_t Write_Memory_1_Byte(uint16_t addr, uint8_t WrData);

/**@brief       ��洢����ȡ 1 �ֽ�����
* @param[in]    addr : ��ȡ��ַ
* @return       ����ִ�н��
* - 1 �ֽ�����
* @note         ��������Ҫ������Ӷ�ȡ�洢����API����
*/
uint8_t Read_Memory_1_Byte(uint16_t addr);

/**@brief       ��洢��д�� 1 �ֽ�����
* @param[in]    addr : д���ַ
* @param[in]    WrData : д�������
* @return       ����ִ�н��
* - OP_SUCCESS(�ɹ�)
* - OP_FAILED(ʧ��)
* @note         ��������Ҫ�������д��洢����API����,
* ������ӵ���доƬ�ڲ�Flash����
*/
void HandleFrameInformation(ModBus_Param_TypeDef *ModBus_Param);

#ifdef __cplusplus
}
#endif
#endif

