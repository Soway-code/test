/**@file        In_Flash_app.h
* @details      In_Flash_app.c��ͷ�ļ�,�������ڲ�FlashӦ�õ�API����
* @author       ���
* @date         2020-04-30
* @version      V1.0.0
* @copyright    2020-2030,��������Ϊ�Ƽ���չ���޹�˾
**********************************************************************************
* @par �޸���־:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/04/30  <td>1.0.0    <td>���    <td>������ʼ�汾
* </table>
*
**********************************************************************************
*/
#ifndef __IN_FLASH_APP_H
#define __IN_FLASH_APP_H

#include "In_Flash.h"
#include "TypeConvert.h"

/***************************** �豸�������ڲ�Flash�е�ӳ���ַ ****************************/
#define RUN_ADDR_BASE                     0x00                                  ///< ϵͳ������ʼ����־/�ڲ�Flash��ʼ��ַ
#define DEVICE_ADDR                       (RUN_ADDR_BASE + 0x01)                ///< �豸��ַ
#define BAUDRATE                          (DEVICE_ADDR   + 0x01)                ///< ������
#define PARITY                            (BAUDRATE      + 0x01)                ///< ��żУ��
#define FILTER                            (PARITY        + 0x01)                ///< �˲�ϵ��
#define AUTO_UPLOAD                       (FILTER        + 0x01)                ///< �Զ��ϴ�����
#define COMPENSATE                        (AUTO_UPLOAD   + 0x01)                ///< ����ʹ��
#define FREEZE                            (COMPENSATE    + 0x01)                ///< �Ƿ񶳽��豸
#define OUTPUTMODE                        (FREEZE        + 0x01)                ///< �����ʽ
#define HEIGHTRANGE                       (OUTPUTMODE    + 0x01)                ///< ����
#define CAPMIN                            (HEIGHTRANGE   + 0x02)                ///< ������
#define CAPMAX                            (CAPMIN        + 0x04)                ///< �����̵���
#define CAPMINBAK                         (CAPMAX        + 0x04)                ///< �����ݱ���
#define CAPMAXBAK                         (CAPMINBAK     + 0x04)                ///< �����̵��ݱ���
#define CAPADMIN                          (CAPMAXBAK     + 0x04)                ///< �������ADֵ
#define CAPADLOW                          (CAPADMIN      + 0x02)                ///< �����¿̶�ADֵ
#define CAPADHIGH                         (CAPADLOW      + 0x02)                ///< �����Ͽ̶�ADֵ
#define CAPADMAX                          (CAPADHIGH     + 0x02)                ///< ����������ADֵ
#define CAPDAMIN                          (CAPADMAX      + 0x02)                ///< �������DAֵ
#define CAPDALOW                          (CAPDAMIN      + 0x02)                ///< �����¿̶�DAֵ
#define CAPDAHIGH                         (CAPDALOW      + 0x02)                ///< �����Ͽ̶�DAֵ
#define CAPDAMAX                          (CAPDAHIGH     + 0x02)                ///< ����������DAֵ
#define TEMPDAMIN                         (CAPDAMAX      + 0x02)                ///< �¶����DAֵ
#define TEMPDAMAX                         (TEMPDAMIN     + 0x02)                ///< �¶�������DAֵ
#define CORRECT_K                         (TEMPDAMAX     + 0x02)                ///< ��������ϵ��K
#define CORRECT_B                         (CORRECT_K     + 0x02)                ///< ��������ϵ��B
#define TEMPER_K1                         (CORRECT_B     + 0x02)                ///< �¶�1����ϵ��K1
#define TEMPER_B1                         (TEMPER_K1     + 0x02)                ///< �¶�1����ϵ��B1
#define TEMPER_K2                         (TEMPER_B1     + 0x02)                ///< �¶�2����ϵ��K2
#define TEMPER_B2                         (TEMPER_K2     + 0x02)                ///< �¶�2����ϵ��B2
#define SYSTEMPARAM_PROGRAMED             (TEMPER_B2     + 0x02)                /** ���ڼ��Flash�궨������
                                                                                �Ƿ�д�����ʼֵ��ֵΪ
                                                                                0xAA��д��� */
#define SYSTEM_PARAM_BAK1                 (RUN_ADDR_BASE     + FLASH_PAGE_SIZE) ///< ϵͳ��������1��ʼ��ַ
#define SYSTEM_PARAM_BAK2                 (SYSTEM_PARAM_BAK1 + FLASH_PAGE_SIZE) ///< ϵͳ��������2��ʼ��ַ     

#define ORGANIZATION                       0x0400                               ///< ��֯��������
#define PRODUCTION                        (ORGANIZATION  + 0x30)                ///< ��Ʒ����
#define HARDWAREVER                       (PRODUCTION    + 0x30)                ///< Ӳ���汾
#define SOFTWAREVER                       (HARDWAREVER   + 0x30)                ///< ����汾
#define DEVICENUM                         (SOFTWAREVER   + 0x20)                ///< �豸ID
#define CUSTOMER                          (DEVICENUM     + 0x30)                ///< �ͻ�����


/***************************** �豸����ռ�ô洢�ռ�ĳ��� ****************************/                                                                      
#define USER_DEFAULT_LEN                  (OUTPUTMODE    + 0x01)                ///< ���������ܳ���   
#define PRO_DEFAULT_LEN                   (SYSTEMPARAM_PROGRAMED + 0x01)        ///< ȫ�������ܳ���   


/********************************** Flashд���־ֵ *********************************/   
#define SYSTEMPARAM_IS_PROGRAMED          0xAA                                  ///< д���ʼֵ��־



/**@brief       ��STM32F072xx�ڲ�Flashָ��λ��д����ֽ��ұ���3��
* @param[in]    FlashAddr : д��ʼ��ַ
* @param[in]    pWrbuf : Ҫд������ݻ���ָ��;
* @param[in]    Wrlen : ���ݳ���
* @return       ����ִ�н��
* - OP_SUCCESS(�ɹ�)
* - OP_FAILED(ʧ��)
*/
uint8_t InFlash_Write3T_MultiBytes(uint16_t FlashAddr, const uint8_t *pWrbuf, uint16_t Wrlen);

/**@brief       �ڲ�Flashϵͳ�������,�����ֲ�һ�µĲ���,���½�User_Default_Param
* д���ڲ�Flash
* @param[in]    Cur_Param : Ҫ���Ĳ�������;
* @param[in]    Num_Of_Cur_Param : ��������ĳ���;
* @return       ����ִ�н��
* - None
*/
void InFlash_SystemParam_Check(uint8_t *Cur_Param, uint16_t Num_Of_Cur_Param);

/**@brief       ��ȡ����鱣�����ڲ�Flash���豸����
* @return       ����ִ�н��
* - None
*/
void Check_Device_Param(void);

#endif
