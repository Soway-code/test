/**@file        Modbus_Conf.h
* @details      Modbus_Conf.c��ͷ�ļ�,�����˵��ݴ������궨��API����,������
* �豸�����ṹ��
* @author       ���
* @date         2020-04-29
* @version      V1.0.0
* @copyright    2020-2030,��������Ϊ�Ƽ���չ���޹�˾
**********************************************************************************
* @par �޸���־:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/04/29  <td>1.0.0    <td>���    <td>������ʼ�汾
* </table>
*
**********************************************************************************
*/

#ifndef __MODBUS_CONF_H
#define __MODBUS_CONF_H
#ifdef __cplusplus
 extern "C" {
#endif


#include "Picocap_app.h"
#include "adc_app.h"


#define DAC_VALUE_MAX   4095        ///< DAC���ֵ

/** ModBus�����豸�Ĳ����ṹ,�ɸ��ݲ�ͬ�Ĳ�Ʒ�����ɾ����Ա */
typedef struct {
    DataFilterParam *DataFilter;
    PCap_DataConvert_Param *PCap_DataConvert;
    PCap_DataConvert_Out_Param *PCap_DataConvert_Out; 
    ADC_TemperParam_TypeDef *ADC_TemperParam;
    ADC_TemperOut_TypeDef   *ADC_TemperOut;
	uint32_t 				temperature;		//���18B20�¶�
}ModBus_Device_Param;


/**@brief       ���ݱ궨
* @param[in]    arg : �û��Զ���Ĳ���,����Ϊ�豸����
* @return       ����ִ�н��
* - None
*/
void MB_Cap_Calibration(void *arg);

/**@brief       ����ADֵ�궨
* @param[in]    arg : �û��Զ���Ĳ���,����Ϊ�豸����
* @return       ����ִ�н��
* - None
*/
void MB_CapAD_Calibration(void *arg);

/**@brief       ����DAֵ�궨
* @param[in]    arg : �û��Զ���Ĳ���,����Ϊ�豸����
* @return       ����ִ�н��
* - None
*/
void MB_CapDAOut_Calibration(void *arg);

/**@brief       �¶�DAֵ�궨����
* @param[in]    arg : �û��Զ���Ĳ���,����Ϊ�豸����
* @return       ����ִ�н��
* - None
*/
void MB_TempDAOut_Calibration(void *arg);

#ifdef __cplusplus
}
#endif
#endif
