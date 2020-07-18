/**@file        menu_uart.c
* @brief        �������ݰ�����
* @details      ����λ���·������ݽ��н���������, ʵ�ֽ�APP�������ص�оƬ�Ĺ���
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

/* Includes ------------------------------------------------------------------*/
#include "common.h"
#include "flash_if.h"
#include "menu_uart.h"
#include "In_Flash.h"



/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/                        

static uint8_t SendLen;                                 ///< ���ڷ��ͳ���
static uint16_t PacketCnt;                              ///< �����
static uint16_t PacketNum;                              ///< �ܰ���

static uint32_t Flashadrdst;                            ///< FLASH��ַ
static uint32_t FileCheckSum;                           ///< �����ļ�У���
static uint32_t FileRunCheckSum;                        ///< �����ļ�ʵʱУ���

static uint8_t SendBuf[USART_ASCII_MIN_DATALEN / 2];    ///< ���ڷ�����ʱ����



/**@brief       ��洢��д�� 1 �ֽ�����
* @param[in]    addr : д���ַ
* @param[in]    WrData : д�������
* @return       ����ִ�н��
* - OP_SUCCESS(�ɹ�)
* - OP_FAILED(ʧ��)
* @note         ��������Ҫ�������д��洢����API����,
* ������ӵ���доƬ�ڲ�Flash����
*/
uint8_t Write_Memory_1_Byte(uint16_t addr, uint8_t WrData)
{
    return InFlash_Write_OneByte(addr, WrData);
}

/**@brief       ��洢����ȡ 1 �ֽ�����
* @param[in]    addr : ��ȡ��ַ
* @return       ����ִ�н��
* - 1 �ֽ�����
* @note         ��������Ҫ������Ӷ�ȡ�洢����API����,
* ������ӵ��Ƕ�оƬ�ڲ�Flash����
*/
uint8_t Read_Memory_1_Byte(uint16_t addr)
{
    return InFlash_Read_OneByte(addr);
}

/**@brief       ��������������
* @param[in]    ModBus_Param : ModBus����Ļ��������ṹ��
* @return       ����ִ�н��
* - None
*/
void HandleFrameInformation(ModBus_Param_TypeDef *ModBus_Param)
{
    uint8_t fmerror;                                                              //֡����
    uint8_t tmpaddr;                                                              //�ݴ��ַ
    uint16_t rcvlen;                                                              //�������ݳ���
    uint16_t prt;                                                                 //����
    uint16_t msgid;                                                               //��ϢID
    uint16_t datalen;                                                             //���ݳ���
    uint16_t packetnum = 0;                                                     //�ܰ���
    uint16_t packetcnt;                                                           //�����
    uint32_t tpcksum;                                                             //��У���
    uint32_t *ramdata;                                                            //����ָ��

    fmerror = MODBUS_ASCII_RecvData(ModBus_Param->RX_buf, &rcvlen);                                  //�������ݴ����򲻷���

    if(0x41 != ModBus_Param->RX_buf[1])      //����������򲻷���
    {
        return ;
    }

    tmpaddr = ModBus_Param->RX_buf[0];                                                               //��ȡ�ӻ���ַ
    msgid = ModBus_Param->RX_buf[2] * 256 + ModBus_Param->RX_buf[3];                                           //��ȡ��ϢID
    datalen = ModBus_Param->RX_buf[4] * 256 + ModBus_Param->RX_buf[5];                                         //��ȡ���ݳ���

    if(tmpaddr == 0x00 || tmpaddr != ModBus_Param->Device_Addr)
    {
        return ;
    }
    if((rcvlen - 0x06) != datalen)                                                          //���ݳ��Ȳ���ȷ�򲻷���
    {
        return ;
    }

    if((ModBus_Param->Device_Addr != ModBus_Param->RX_buf[0])
      &&(0xFF != ModBus_Param->RX_buf[0]))                          //�ӻ���ַ��������Χ������
    {
        return ;
    }

    if((ModBus_Param->UpgradeWaitTime != -1) && (msgid != 0x0004) && (msgid != 0x0001))                         //����������Ϣ�йز�����
    {
        return ;
    }

    for(SendLen = 0; SendLen < 4; SendLen++)                                                 //�洢��������Ϣ
    {
        SendBuf[SendLen] = ModBus_Param->RX_buf[SendLen];
    }

    SendBuf[SendLen++] = 0x00;
    SendBuf[SendLen++] = 0x01;
    if(fmerror == ERR_NONE)
    {
        switch(msgid)
        {
        case 0x0001:                                                                                //��ʼ����      
          ModBus_Param->UpgradeWaitTime = -1;        
          if((0 != datalen) && (2 != datalen))
          {
            SendBuf[SendLen++] = RESPONSE_REC_ERR;
            break;
          }
          else 
          {
              if(datalen == 2)
              {
                  packetnum = ModBus_Param->RX_buf[6] * 256 + ModBus_Param->RX_buf[7];    //��ȡ�ܰ���
              }
              SendBuf[SendLen++] = RESPONSE_ERR_NONE;
          }  
          prt = 0;
          PacketNum = 0;
          PacketCnt = 0;
          
          if(packetnum > 0)
          {           
              InFlash_Erase_Page(0, 1);
          }
          break;
          
        case 0x0002:                                                                                //���Դ����
          if(0 != datalen)
          {
            SendBuf[SendLen++] = RESPONSE_REC_ERR;
            break;
          }          
          FLASH_If_Init();                                                                          //FLASH����
          if(FLASH_If_GetWriteProtectionStatus() != FLASHIF_PROTECTION_NONE)
          {
            if(FLASH_If_WriteProtectionConfig(FLASHIF_WRP_DISABLE) == FLASHIF_OK)
            {
              HAL_FLASH_OB_Launch();
            }
          }
          FLASH_If_Erase(APPLICATION_ADDRESS);
          Flashadrdst = APPLICATION_ADDRESS;
          ModBus_Param->ProgErase = ERASE_FLAG;
          SendBuf[SendLen++] = RESPONSE_ERR_NONE;
          Write_Memory_1_Byte(ADDR_ERASEFLAG, ERASE_FLAG);
          break;

        case 0x0003:                //���������ļ�
            packetnum = ModBus_Param->RX_buf[6] * 256 + ModBus_Param->RX_buf[7];    //��ȡ�ܰ���
            packetcnt = ModBus_Param->RX_buf[8] * 256 + ModBus_Param->RX_buf[9];
          if((0 == PacketNum) && (1 < packetnum) && (0 == packetcnt))
          {
            FileCheckSum = 0;
            FileRunCheckSum = 0;
            PacketNum = packetnum;
            PacketCnt = packetcnt;
            SendBuf[SendLen++] = RESPONSE_ERR_NONE;
          
            for(prt = 0; prt < 4; prt++)
            {
              FileCheckSum <<= 8;
              FileCheckSum += ModBus_Param->RX_buf[10 + prt];
            }
          }
          else if((PacketNum == packetnum) && (1 < packetnum) 
              && (PacketCnt == (packetcnt - 1)) && (PacketNum > packetcnt))
          {
            tpcksum = 0;
            datalen = datalen - 4;
          
            for(prt = 0; prt < datalen; prt++)
            {
              tpcksum += ModBus_Param->RX_buf[10 + prt];
            }
            
            Decoding(&ModBus_Param->RX_buf[10], datalen);
            
            for(prt = 0; prt < datalen; prt++)
            {
              ModBus_Param->RX_buf[prt] = ModBus_Param->RX_buf[10 + prt];
            }
            ramdata = (uint32_t*)ModBus_Param->RX_buf;
          
            if(FLASH_If_Write(Flashadrdst, ramdata, datalen/4)  == 0)
            {
              PacketCnt++;
              SendBuf[SendLen++] = RESPONSE_ERR_NONE;
              FileRunCheckSum += tpcksum;
                Flashadrdst += datalen;
            }
            else
            {
              SendBuf[SendLen++] = RESPONSE_REC_ERR;
            }
          }
          else if((PacketNum == packetnum) && (1 < packetnum) 
              && (PacketCnt == packetcnt) && (PacketNum > packetcnt))
          {
            SendBuf[SendLen++] = RESPONSE_ERR_NONE;
          }
          else
          {
            SendBuf[SendLen++] = RESPONSE_REC_ERR;
          }
          break;

        case 0x0004:                                                                                  //ִ��Ӧ�ó���
          if((((FileRunCheckSum == FileCheckSum) && ((PacketCnt + 1) == PacketNum)) 
              || (0 == PacketNum))&&(0 == datalen))
          {
            if(0 != PacketNum)
            {
              ModBus_Param->UpgradeWaitTime = 0;
              SendBuf[SendLen++] = RESPONSE_ERR_NONE;
            }
            else if((ModBus_Param->ProgErase == ERASE_FLAG)
                || (ModBus_Param->ProgErase == ERASE_FLAG_NONE))
            {
              SendBuf[SendLen++] = RESPONSE_REC_ERR;
            }
            else
            {
              ModBus_Param->UpgradeWaitTime = 0;
              SendBuf[SendLen++] = RESPONSE_ERR_NONE;
            }
          }
          else
          {
            SendBuf[SendLen++] = RESPONSE_LRC_ERR;
          }
          break;

        default:
          SendBuf[SendLen++] = RESPONSE_REC_ERR;
          break;
        }
    }
    else
    {
        SendBuf[SendLen++] = RESPONSE_LRC_ERR;         //У�����
    }

    MODBUS_ASCII_SendData(SendBuf, SendLen);      
}

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
