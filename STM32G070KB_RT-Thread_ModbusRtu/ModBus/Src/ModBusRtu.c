/**@file        ModbusRtu.c
* @brief        Modbus RTU ͨ��Э��
* @details      ��д�������Ĵ����궨���ܡ��ɼ����ݽ���
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

#include "ModBusRtu.h"


///< CRC_16�����ѯ��
/** CRC��λ�ֽ�ֵ�� */
const uint8_t auchCRCLo[256] =
{
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
    0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};

/** CRC��λ�ֽ�ֵ�� */
const uint8_t auchCRCHi[256] =
{
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
    0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
    0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
    0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
    0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
    0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
    0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
    0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
    0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
    0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
    0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
    0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
    0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
    0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
    0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
    0x43, 0x83, 0x41, 0x81, 0x80, 0x40
};

static uint8_t ModBus_Receive_Buf[RECEIVE_SIZE];    ///< ModBus���ջ���(ModBus��������ʱ,ʹ���������)
static uint8_t ModBus_Send_Buf[SEND_SIZE];          ///< ModBus���ͻ���

#ifdef __IN_FLASH_APP_H

/**@brief       ModBus��ʼ��
* @param[in]    ModBusBaseParam : ModBus����Ļ��������ṹ��
* @return       ����ִ�н��
* - None
*/
void ModBus_Init(ModBusBaseParam_TypeDef *ModBusBaseParam)
{
    uint8_t Read_Data;
    
    InFlash_Read_MultiBytes(DEVICE_ADDR, &Read_Data, 1);    //�豸��ַ
    ModBusBaseParam->Device_Addr = Read_Data;
    
    InFlash_Read_MultiBytes(BAUDRATE, &Read_Data, 1);       //���ڲ�����
    ModBusBaseParam->BaudRate = Read_Data;
	
    InFlash_Read_MultiBytes(PARITY, &Read_Data, 1);         //������żУ��
    ModBusBaseParam->Parity = Read_Data;
    
    InFlash_Read_MultiBytes(AUTO_UPLOAD, &Read_Data, 1);    //�Զ��ϴ�����
    ModBusBaseParam->AutoUpload = Read_Data;
    
    InFlash_Read_MultiBytes(OUTPUTMODE, &Read_Data, 1);     //���ģʽ
    ModBusBaseParam->Output_Mode = Read_Data;    
    
    InFlash_Read_MultiBytes(FREEZE, &Read_Data, 1);         //�豸�����־
    ModBusBaseParam->Freeze = Read_Data;
    
    ModBusBaseParam->InRomWrEn = IN_FLASH_WR_DISABLE;          //д�ڲ�Flashʹ�ܱ�־
    ModBusBaseParam->ModBus_CallBack = NULL;                //ModBus�ص�����
    
    ModBusBaseParam->ModBus_TX_RX.Receive_Buf = ModBus_Receive_Buf;         //ModBus�������ݻ���
    ModBusBaseParam->ModBus_TX_RX.Send_Buf = ModBus_Send_Buf;               //ModBus�������ݻ���
    ModBusBaseParam->ModBus_TX_RX.Send_Len = 0;                             //ModBus�������ݻ��泤��
    
    Sensor_USART_Init(ModBusBaseParam->BaudRate, ModBusBaseParam->Parity);
}
#else

/**@brief       ModBus��ʼ��
* @param[in]    ModBusBaseParam : ModBus����Ļ��������ṹ��
* @param[in]    Param : ModBus��ʼ������������
* @param[in]    Param_Size : Param�����ݳ���
* @return       ����ִ�н��
* - OP_SUCCESS(�ɹ�)
* - OP_FAILED(ʧ��)
* @note         Param����������(���ֽ�˳������): 0,�豸��ַ; 
* 1,�����ʴ���; 2��żУ�����; 3�Զ��ϴ�����; 4,���ģʽ;
* 5,�豸�����־;
*/
uint8_t ModBus_Init(ModBusBaseParam_TypeDef *ModBusBaseParam,
                uint8_t *Param, uint16_t Param_Size)
{    
    uint8_t cnt = 0;

    if(Param != NULL && Param_Size != 0)
    {
        ModBusBaseParam->Device_Addr = Param[cnt++];
        ModBusBaseParam->BaudRate = Param[cnt++];
        ModBusBaseParam->Parity = Param[cnt++];
        ModBusBaseParam->AutoUpload = Param[cnt++];        
        ModBusBaseParam->Output_Mode = Param[cnt++];
        ModBusBaseParam->Freeze = Param[cnt++];        
        ModBusBaseParam->InRomWrEn = FLASH_WR_DISABLE;
        ModBusBaseParam->ModBus_CallBack = NULL;
        
        ModBusBaseParam->ModBus_TX_RX.Receive_Buf = ModBus_Receive_Buf;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf = ModBus_Send_Buf;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 0;
        
        Sensor_USART_Init(ModBusBaseParam->BaudRate, ModBusBaseParam->Parity);
        
        if(cnt > Param_Size)
        {
            return OP_FAILED;
        }
        return OP_SUCCESS;
    }
    return OP_FAILED;
}
#endif

/**@brief       ���������Ϣ֡����CRC16У��
* @param[in]    Msg : ��Ϣ�׵�ַ;
* @param[in]    MsgLen : ��Ϣ֡����;
* @return       ����ִ�н��
* - CRC16У����
* @note         ��Ϣ���Ȳ���Ϊ0
*/
uint16_t MDB_Get_CRC16(uint8_t *Msg, uint16_t MsgLen) 
{
    uint8_t CRCLo = 0xFF; 
    uint8_t CRCHi = 0xFF; 
    uint8_t uIndex;  
    uIndex = 0;

    while (MsgLen--)  
    { 
        uIndex = CRCLo ^ *Msg++; 
        CRCLo  = CRCHi ^ auchCRCLo[uIndex]; 
        CRCHi  = auchCRCHi[uIndex]; 
    } 

    return (CRCHi << 8) | CRCLo;
}

/**@brief       ���� Modbus RTU��Ϣ֡
* @param[in]    ModBusBaseParam : ModBus����Ļ��������ṹ��;
* @param[in]    Msg : ��Ϣ�׵�ַ; 
* @param[in]    MsgLen : ��Ϣ֡����;
* @param[in]    Check_Addr : �Ƿ����豸��ַ��Ч
* @return       ����ִ�н��
* - None
* @note         ��Ϣ���Ȳ���Ϊ0������ʱ�Ƚⶳ�豸
*/
void MDB_Snd_Msg_RTU(ModBusBaseParam_TypeDef *ModBusBaseParam, uint8_t *pMsg, uint16_t len, uint8_t Check_Addr)
{
    uint8_t ucCRC[2];
    uint16_t uiCRC;
    //��Ч��ַ�������ݳ��ȳ���
    if(((uint8_t *)0 == pMsg) || (0 == len) || ((SEND_SIZE - 2) < len))
    {
        return;
    }
    //�㲥��ַ
    if((BROADCASTADDR == pMsg[0]) && Check_Addr)
    {
        return;
    }
    //�豸������
    if(FREEZE_DISABLE != ModBusBaseParam->Freeze)
    {
        return;
    }
    //�õ�CRC��
    uiCRC = MDB_Get_CRC16(pMsg, len);
    //CRC���ֽ���ǰ
    ucCRC[0] = (uint8_t)uiCRC;     
    //CRC���ֽ��ں�
    ucCRC[1] = (uint8_t)(uiCRC >> 8);     

    pMsg[len] = ucCRC[0];
    pMsg[len+1] = ucCRC[1];

    //��������
    Send_Data(ModBusBaseParam, pMsg, len + 2);
}

/**@brief       ��������
* @param[in]    ModBusBaseParam : ModBus����Ļ��������ṹ��;
* @param[in]    Msg : ��Ϣ�׵�ַ;
* @param[in]    MsgLen : ��Ϣ֡����;
* @return       ����ִ�н��
* - None
*/
void Send_Data(ModBusBaseParam_TypeDef *ModBusBaseParam, uint8_t *pMsg, uint16_t len)
{
#ifdef  USART_USING_485
    //����ʹ��
    TX_ON;
#endif
/* ʹ��RT-Thread����ϵͳ,USING_RT_THREAD_OS��main.h�ж��� */
#ifdef USING_RT_THREAD_OS
    rt_sem_take(ModBusBaseParam->TX_Lock, RT_WAITING_FOREVER);    //��ȡ�ź���
#else
    while(Sensor_USART_Get_TX_Cplt_Flag() == 0);
    Sensor_USART_Clear_TX_Cplt_Flag();
#endif
    //DMA����
    BSP_UART_Transmit_DMA(pMsg, len);        
}

/**@brief       Modbus RTU��Ϣ֡������
* @param[in]    ModBusBaseParam : ModBus����Ļ��������ṹ��;
* @param[in]    arg : �û��Զ���Ĳ���,�����豸����
* @return       ����ִ�н��
* - None
*/
void ModbusHandle(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg)
{
    uint16_t crc;
    uint16_t recv_len;
        
    //��ȡһ֡ModBus����
    recv_len = Sensor_USART_GetReceData(ModBusBaseParam->ModBus_TX_RX.Receive_Buf);   
    if((recv_len - 2) < 0)         //���յ����ݳ������� >= 2
    {
        return;
    }
    //��ȡCRCֵ
    crc = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[recv_len - 1] << 8;
    crc |= ModBusBaseParam->ModBus_TX_RX.Receive_Buf[recv_len - 2];
    //У����󣬵�ַ���󶼲�������Ϣ֡
    if( crc != MDB_Get_CRC16(ModBusBaseParam->ModBus_TX_RX.Receive_Buf, recv_len - 2) 
        || ((ModBusBaseParam->Device_Addr != ModBusBaseParam->ModBus_TX_RX.Receive_Buf[0]) 
        && (BROADCASTADDR != ModBusBaseParam->ModBus_TX_RX.Receive_Buf[0])))
    {     
        return;
    }
    switch(ModBusBaseParam->ModBus_TX_RX.Receive_Buf[1])    //�����봦��
    {
    case 0x03:
        ModbusFunc03(ModBusBaseParam, arg);
    break;

    case 0x04:
        ModbusFunc04(ModBusBaseParam, arg);
    break;

    case 0x05:
        ModbusFunc05(ModBusBaseParam, arg);
    break;
 
    case 0x10:
        ModbusFunc10(ModBusBaseParam, arg);
    break;

    case 0x25:
        ModbusFunc25(ModBusBaseParam, arg);
    break;
  
    case 0x26:
        ModbusFunc26(ModBusBaseParam, arg);
    break;
   
    case 0x27:
        ModbusFunc27(ModBusBaseParam, arg);
    break;
 
    case 0x2A:
        ModbusFunc2A(ModBusBaseParam);
    break;

    case 0x2B:
        ModbusFunc2B(ModBusBaseParam);
    break;
      
    case 0x41:
        ModbusFunc41(ModBusBaseParam);
    break;   

    default:
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[0] 
            = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[0];
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] 
            = (ModBusBaseParam->ModBus_TX_RX.Receive_Buf[1] | MB_REQ_FAILURE);
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_FUNC_EXCEPTION;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
    break;
    }
    //����Modbus RTU
    MDB_Snd_Msg_RTU(ModBusBaseParam, 
                    ModBusBaseParam->ModBus_TX_RX.Send_Buf, 
                    ModBusBaseParam->ModBus_TX_RX.Send_Len,
                    CHECK_ADDRESS);
    if(ModBusBaseParam->ModBus_CallBack != NULL)
    {        
        ModBusBaseParam->ModBus_CallBack(ModBusBaseParam);
        ModBusBaseParam->ModBus_CallBack = NULL;
    }    
}

/**@brief       Modbus 03��������Ϣ֡����
* @param[in]    ModBusBaseParam : ModBus����Ļ��������ṹ��;
* @param[in]    arg : �û��Զ���Ĳ���,�����豸����
* @return       ����ִ�н��
* - None
* @note         ����ʹ��������,ʵ�ʺ�����ModBus_Conf.c���涨��
*/
__weak void ModbusFunc03(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg)
{
    UNUSED(ModBusBaseParam);
    UNUSED(arg);
}

/**@brief       Modbus 04��������Ϣ֡����
* @param[in]    ModBusBaseParam : ModBus����Ļ��������ṹ��;
* @param[in]    arg : �û��Զ���Ĳ���,�����豸����
* @return       ����ִ�н��
* - None
* @note         ����ʹ��������,ʵ�ʺ�����ModBus_Conf.c���涨��
*/
__weak void ModbusFunc04(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg)
{
    UNUSED(ModBusBaseParam);
    UNUSED(arg);
}

/**@brief       Modbus 05��������Ϣ֡����
* @param[in]    ModBusBaseParam : ModBus����Ļ��������ṹ��;
* @param[in]    arg : �û��Զ���Ĳ���,�����豸����
* @return       ����ִ�н��
* - None
* @note         ����ʹ��������,ʵ�ʺ�����ModBus_Conf.c���涨��
*/
__weak void ModbusFunc05(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg)
{
    UNUSED(ModBusBaseParam);
    UNUSED(arg);
}

/**@brief       Modbus 10��������Ϣ֡����
* @param[in]    ModBusBaseParam : ModBus����Ļ��������ṹ��;
* @param[in]    arg : �û��Զ���Ĳ���,�����豸����
* @return       ����ִ�н��
* - None
* @note         ����ʹ��������,ʵ�ʺ�����ModBus_Conf.c���涨��
*/
__weak void ModbusFunc10(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg)
{
    UNUSED(ModBusBaseParam);
    UNUSED(arg);
}

/**@brief       Modbus 25��������Ϣ֡����
* @param[in]    ModBusBaseParam : ModBus����Ļ��������ṹ��;
* @param[in]    arg : �û��Զ���Ĳ���,�����豸����
* @return       ����ִ�н��
* - None
* @note         ����ʹ��������,ʵ�ʺ�����ModBus_Conf.c���涨��
*/
__weak void ModbusFunc25(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg)
{
    UNUSED(ModBusBaseParam);
    UNUSED(arg);
}

/**@brief       Modbus 26��������Ϣ֡����
* @param[in]    ModBusBaseParam : ModBus����Ļ��������ṹ��;
* @param[in]    arg : �û��Զ���Ĳ���,�����豸����
* @return       ����ִ�н��
* - None
* @note         ����ʹ��������,ʵ�ʺ�����ModBus_Conf.c���涨��
*/
__weak void ModbusFunc26(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg)
{
    UNUSED(ModBusBaseParam);
    UNUSED(arg);
}

/**@brief       Modbus 27��������Ϣ֡����
* @param[in]    ModBusBaseParam : ModBus����Ļ��������ṹ��;
* @param[in]    arg : �û��Զ���Ĳ���,�����豸����
* @return       ����ִ�н��
* - None
* @note         ����ʹ��������,ʵ�ʺ�����ModBus_Conf.c���涨��
*/
__weak void ModbusFunc27(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg)
{
    UNUSED(ModBusBaseParam);
    UNUSED(arg);
}

/**@brief       Modbus 2A��������Ϣ֡����
* @param[in]    ModBusBaseParam : ModBus����Ļ��������ṹ��;
* @return       ����ִ�н��
* - None
* @note         ����ʹ��������,ʵ�ʺ�����ModBus_Conf.c���涨��
*/
__weak void ModbusFunc2A(ModBusBaseParam_TypeDef *ModBusBaseParam)
{
    UNUSED(ModBusBaseParam);
}

/**@brief       Modbus 2B��������Ϣ֡����
* @param[in]    ModBusBaseParam : ModBus����Ļ��������ṹ��;
* @return       ����ִ�н��
* - None
* @note         ����ʹ��������,ʵ�ʺ�����ModBus_Conf.c���涨��
*/
__weak void ModbusFunc2B(ModBusBaseParam_TypeDef *ModBusBaseParam)
{
    UNUSED(ModBusBaseParam);
}

/**@brief       Modbus 41��������Ϣ֡����
* @param[in]    ModBusBaseParam : ModBus����Ļ��������ṹ��;
* @return       ����ִ�н��
* - None
* @note         ����ʹ��������,ʵ�ʺ�����ModBus_Conf.c���涨��
*/
__weak void ModbusFunc41(ModBusBaseParam_TypeDef *ModBusBaseParam)
{
    UNUSED(ModBusBaseParam);
}

/**@brief       Modbus ��Ϣ֡�Զ��ϴ�����
* @param[in]    ModBusBaseParam : ModBus����Ļ��������ṹ��;
* @param[in]    arg : �û��Զ���Ĳ���,�����豸����
* @return       ����ִ�н��
* - None
* @note         ����ʹ��������,ʵ�ʺ�����ModBus_Conf.c���涨��
*/
__weak void ModbusAutoUpload(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg)
{
    UNUSED(ModBusBaseParam);
    UNUSED(arg);
}

/**@brief       �������¼�����,�Զ��ϴ����ڲ�Ϊ0ʱ,�����Եص���ModbusAutoUpload
* @param[in]    ModBusBaseParam : ModBus����Ļ��������ṹ��;
* @param[in]    arg : �û��Զ���Ĳ���,�����豸����
* @return       ����ִ�н��
* - None
*/
void SensorEvent(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg)
{
    static uint32_t Old_Tick = 0;
    //���ʹ���Զ��ϴ����Զ��ϴ���ʱʱ�䵽
    if((0 != ModBusBaseParam->AutoUpload) 
        && ((ModBusBaseParam->AutoUpload * AUTOUPLOAD_CYCLE) <= (HAL_GetTick() - Old_Tick)))
    {
        //Modbus֡�Զ��ϴ�
        ModbusAutoUpload(ModBusBaseParam, arg);
        Old_Tick = HAL_GetTick();
    }
}

/* ʹ��RT-Thread����ϵͳ,USING_RT_THREAD_OS��main.h�ж��� */
#ifdef USING_RT_THREAD_OS
#include "rtthread.h"

#define MODBUS_HANDLE_THREAD_STACK          512
#define MODBUS_HANDLE_THREAD_PRIORITY       RT_THREAD_PRIORITY_MAX - 4

#define MODBUS_AUTOUPLOAD_THREAD_STACK      512
#define MODBUS_AUTOUPLOAD_THREAD_PRIORITY   RT_THREAD_PRIORITY_MAX - 3

static ModBusBaseParam_TypeDef     ModBusBase_Param;     //ModBus����Ļ��������ṹ
static ModBus_Device_Param         ModBus_Device;       //ModBus�����豸�Ĳ����ṹ��


/**@brief       ModBus�����߳�,���ڴ���ModBus��Ϣ֡
* @param[in]    parameter : �̵߳Ĳ���,���ﲻʹ��
* @return       ����ִ�н��
* - None
*/
void modbus_handle_thread_entry(void *parameter)
{                
    while(1)
    {
        //ModBus����
        if(Sensor_USART_Get_RX_Updata_Flag())
        {
            ModbusHandle(&ModBusBase_Param, &ModBus_Device);
            Sensor_USART_Clear_RX_Updata_Flag();
        } 
    }
}

/**@brief       ModBus�Զ��ϴ������߳�,���ڴ���ModBus�Զ��ϴ��¼�
* @param[in]    parameter : �̵߳Ĳ���,���ﲻʹ��
* @return       ����ִ�н��
* - None
*/
void modbus_autoupload_thread_entry(void *parameter)
{
    while(1)
    {
        //����Զ��ϴ�ֵ��0,��ʹ���Զ��ϴ�
        if(ModBusBase_Param.AutoUpload != 0)
        {
            //Modbus֡�Զ��ϴ�
            ModbusAutoUpload(&ModBusBase_Param, &ModBus_Device);        
            rt_thread_mdelay(ModBusBase_Param.AutoUpload * AUTOUPLOAD_CYCLE);
        }
        else
        {
            rt_thread_mdelay(1000);
        }
    }
}

/**@brief       ModBus������ʼ��,����ModBus�����̺߳�ModBus�Զ��ϴ������̲߳�����
* @return       ����ִ�н��
* - int����ֵ(RT_EOK)
* @note         ������ʹ��RT-Thread���Զ���ʼ�����INIT_COMPONENT_EXPORT
* ����ִ��,ϵͳ��λ���Զ���ʼ��
*/
static int modbus_init(void)
{
    rt_device_t                 adc_device;
    rt_device_t                 pcap_device;
    struct rt_adc_device_obj    *adc_device_obj;
    struct rt_pcap_device_obj   *pcap_device_obj;
    rt_thread_t                 modbus_handle_thread;
    rt_thread_t                 modbus_autoupload_thread;
    
    adc_device = rt_device_find(ADC_DEVICE_NAME);
    if(adc_device != RT_NULL)
    {
        adc_device_obj = (struct rt_adc_device_obj *)adc_device->user_data;
        rt_device_open(adc_device, RT_DEVICE_OFLAG_RDONLY);
    }
    pcap_device = rt_device_find(PCAP_DEVICE_NAME);
    if(pcap_device != RT_NULL)
    {
        pcap_device_obj = (struct rt_pcap_device_obj *)pcap_device->user_data;
        rt_device_open(pcap_device, RT_DEVICE_OFLAG_RDWR);
    }        
    
    ModBus_Device.ADC_TemperParam         = &adc_device_obj->ADC_TemperParam;
    ModBus_Device.ADC_TemperOut           = &adc_device_obj->ADC_TemperOut;
    ModBus_Device.DataFilter              = &pcap_device_obj->DataFilter;
    ModBus_Device.PCap_DataConvert        = &pcap_device_obj->PCap_DataConvert; 
    ModBus_Device.PCap_DataConvert_Out    = &pcap_device_obj->PCap_DataConvert_Out;
    
    ModBus_Init(&ModBusBase_Param);                      //ModBus��ʼ��(�������ڳ�ʼ��)    
    
    ModBusBase_Param.TX_Lock
        = (rt_sem_t)rt_object_find(USART_TX_LOCK_NAME, RT_Object_Class_Semaphore);
    
    modbus_handle_thread = rt_thread_create("mb_hand",
                                            modbus_handle_thread_entry,
                                            RT_NULL,
                                            MODBUS_HANDLE_THREAD_STACK,
                                            MODBUS_HANDLE_THREAD_PRIORITY,
                                            20);
    RT_ASSERT(modbus_handle_thread != RT_NULL);
    rt_thread_startup(modbus_handle_thread);
    
    modbus_autoupload_thread = rt_thread_create("mb_up",
                                            modbus_autoupload_thread_entry,
                                            RT_NULL,
                                            MODBUS_AUTOUPLOAD_THREAD_STACK,
                                            MODBUS_AUTOUPLOAD_THREAD_PRIORITY,
                                            20);
    RT_ASSERT(modbus_autoupload_thread != RT_NULL);
    rt_thread_startup(modbus_autoupload_thread);
    
    return RT_EOK;
}
INIT_COMPONENT_EXPORT(modbus_init);

#endif

