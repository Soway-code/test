/**@file        menu_uart.c
* @brief        串口数据包处理
* @details      对上位机下发的数据进行解析、处理, 实现将APP程序下载到芯片的功能
* @author       杨春林
* @date         2020-06-04
* @version      V1.0.0
* @copyright    2020-2030,深圳市信为科技发展有限公司
**********************************************************************************
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/06/04  <td>1.0.0    <td>杨春林    <td>创建初始版本
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

static uint8_t SendLen;                                 ///< 串口发送长度
static uint16_t PacketCnt;                              ///< 包序号
static uint16_t PacketNum;                              ///< 总包数

static uint32_t Flashadrdst;                            ///< FLASH地址
static uint32_t FileCheckSum;                           ///< 升级文件校验和
static uint32_t FileRunCheckSum;                        ///< 升级文件实时校验和

static uint8_t SendBuf[USART_ASCII_MIN_DATALEN / 2];    ///< 串口发送临时缓存



/**@brief       向存储器写入 1 字节数据
* @param[in]    addr : 写入地址
* @param[in]    WrData : 写入的数据
* @return       函数执行结果
* - OP_SUCCESS(成功)
* - OP_FAILED(失败)
* @note         本函数需要自行添加写入存储器的API函数,
* 这里添加的是写芯片内部Flash函数
*/
uint8_t Write_Memory_1_Byte(uint16_t addr, uint8_t WrData)
{
    return InFlash_Write_OneByte(addr, WrData);
}

/**@brief       向存储器读取 1 字节数据
* @param[in]    addr : 读取地址
* @return       函数执行结果
* - 1 字节数据
* @note         本函数需要自行添加读取存储器的API函数,
* 这里添加的是读芯片内部Flash函数
*/
uint8_t Read_Memory_1_Byte(uint16_t addr)
{
    return InFlash_Read_OneByte(addr);
}

/**@brief       处理升级包数据
* @param[in]    ModBus_Param : ModBus处理的基本参数结构体
* @return       函数执行结果
* - None
*/
void HandleFrameInformation(ModBus_Param_TypeDef *ModBus_Param)
{
    uint8_t fmerror;                                                              //帧错误
    uint8_t tmpaddr;                                                              //暂存地址
    uint16_t rcvlen;                                                              //接收数据长度
    uint16_t prt;                                                                 //计数
    uint16_t msgid;                                                               //消息ID
    uint16_t datalen;                                                             //数据长度
    uint16_t packetnum = 0;                                                     //总包数
    uint16_t packetcnt;                                                           //包序号
    uint32_t tpcksum;                                                             //包校验和
    uint32_t *ramdata;                                                            //数据指针

    fmerror = MODBUS_ASCII_RecvData(ModBus_Param->RX_buf, &rcvlen);                                  //接收数据错误则不返回

    if(0x41 != ModBus_Param->RX_buf[1])      //功能码错误则不返回
    {
        return ;
    }

    tmpaddr = ModBus_Param->RX_buf[0];                                                               //获取从机地址
    msgid = ModBus_Param->RX_buf[2] * 256 + ModBus_Param->RX_buf[3];                                           //获取消息ID
    datalen = ModBus_Param->RX_buf[4] * 256 + ModBus_Param->RX_buf[5];                                         //获取数据长度

    if(tmpaddr == 0x00 || tmpaddr != ModBus_Param->Device_Addr)
    {
        return ;
    }
    if((rcvlen - 0x06) != datalen)                                                          //数据长度不正确则不返回
    {
        return ;
    }

    if((ModBus_Param->Device_Addr != ModBus_Param->RX_buf[0])
      &&(0xFF != ModBus_Param->RX_buf[0]))                          //从机地址不在允许范围不处理
    {
        return ;
    }

    if((ModBus_Param->UpgradeWaitTime != -1) && (msgid != 0x0004) && (msgid != 0x0001))                         //不是升级信息有关不处理
    {
        return ;
    }

    for(SendLen = 0; SendLen < 4; SendLen++)                                                 //存储待发送信息
    {
        SendBuf[SendLen] = ModBus_Param->RX_buf[SendLen];
    }

    SendBuf[SendLen++] = 0x00;
    SendBuf[SendLen++] = 0x01;
    if(fmerror == ERR_NONE)
    {
        switch(msgid)
        {
        case 0x0001:                                                                                //开始升级      
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
                  packetnum = ModBus_Param->RX_buf[6] * 256 + ModBus_Param->RX_buf[7];    //获取总包数
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
          
        case 0x0002:                                                                                //清除源程序
          if(0 != datalen)
          {
            SendBuf[SendLen++] = RESPONSE_REC_ERR;
            break;
          }          
          FLASH_If_Init();                                                                          //FLASH解锁
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

        case 0x0003:                //传输升级文件
            packetnum = ModBus_Param->RX_buf[6] * 256 + ModBus_Param->RX_buf[7];    //获取总包数
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

        case 0x0004:                                                                                  //执行应用程序
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
        SendBuf[SendLen++] = RESPONSE_LRC_ERR;         //校验错误
    }

    MODBUS_ASCII_SendData(SendBuf, SendLen);      
}

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
