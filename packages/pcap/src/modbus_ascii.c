#include "bsp.h"
//#include "uffs_types.h"
extern uint32_t get_ds18b20_temp;

extern FlagStatus UartRecvFlag;
extern FlagStatus UartRecvFrameOK ;
extern uint8_t UART1_TXBuff[USART1_MAX_DATALEN];
extern uint8_t UART1_RXBuff[USART1_MAX_DATALEN];     //接收缓冲,最大USART_REC_LEN个字节.
extern void Swy_big_endian_cpy(unsigned char * dest, const void *data, int size);
extern BitAction UartRecvNewData ;
//extern BitAction UartRecvFrameOK ;
static uint8_t g_cyRevState = ASCII_IDLE_STATE;
//static uint16_t g_cyRevBufffLen = 0;
uint8_t UsartErrCnt;
uint8_t cyAsciiBuffc[USART1_MAX_DATALEN];

//static u8 g_cyRevState = ASCII_IDLE_STATE;
int32_t get_temp_value=0;
static uint8_t g_cyRevBufffLen = 0;
extern rt_sem_t cap_sem;
//extern rt_sem_t temp_sem;
extern struct rt_event temp_event;
extern struct rt_event cap_event;
//extern struct rt_event nbtempsend_event;
#define UART_RX_EVENT (1 << 0)
//#define EVENT_FLAG1 (1 << 1)
extern RunVarTypeDef RunVar; 

/************************************************************************************************************************************************************************
** 版权：   2015-2025, 深圳市信为科技发展有限公司
** 文件名:  Modbus_Ascii.h
** 作者:    陈锦
** 版本:    V1.0.0
** 日期:    2015-07-10
** 描述:    各种算法
** 功能:         
*************************************************************************************************************************************************************************
** 修改者:          No
** 版本:  		
** 修改内容:    No 
** 日期:            No
*************************************************************************************************************************************************************************/
//**************************************************************************************************
// 名称         : MODBUS_ASCII_HexToAscii()
// 创建日期     : 2015-07-24
// 作者         : 陈锦
// 功能         : 十六进制数转ASCII码
// 输入参数     : 十六进制数(u8 cyHexData)
// 输出参数     : ASCII码(u8 *pCyAsciiBuf)
// 返回结果     : 无
// 注意和说明   : 
// 修改内容     :
//**************************************************************************************************
void MODBUS_ASCII_HexToAscii(uint8_t cyHexData, uint8_t *pCyAsciiBuf)
{
	uint8_t cyTemp;
	
	cyTemp = cyHexData / 16;
	if (10 > cyTemp) //0-9
	{
		*(pCyAsciiBuf + 0) = cyTemp + '0';
	}
	else
	{
		*(pCyAsciiBuf + 0) = (cyTemp - 10) + 'A';
	}

	cyTemp = cyHexData % 16;
	if (10 > cyTemp) //0-9
	{
		*(pCyAsciiBuf + 1) = cyTemp + '0';
	}
	else
	{
		*(pCyAsciiBuf + 1) = (cyTemp - 10) + 'A';
	}
}


//**************************************************************************************************
// 名称         : MODBUS_ASCII_AsciiToHex()
// 创建日期     : 2015-07-24
// 作者         : 陈锦
// 功能         : ASCII码转十六进制数
// 输入参数     : ASCII码(u8 *pCyAsciiBuf)
// 输出参数     : 无
// 返回结果     : 十六进制数(u8 cyHexData)
// 注意和说明   : 
// 修改内容     :
//**************************************************************************************************
uint8_t MODBUS_ASCII_AsciiToHex(uint8_t *pCyAsciiBuf)
{
	uint8_t cyHexData;
	
	cyHexData = 0;
	if ('A' > *(pCyAsciiBuf + 0) ) //0-9
	{
		cyHexData += *(pCyAsciiBuf + 0) - '0';
	}
	else if ('a' > *(pCyAsciiBuf + 0) ) //大写
	{
		cyHexData += *(pCyAsciiBuf + 0) - 'A' + 10;
	}
	else
	{
		cyHexData += *(pCyAsciiBuf + 0) - 'a' + 10;
	}

	cyHexData *= 16;
	
	if ('A' > *(pCyAsciiBuf + 1) ) //0-9
	{
		cyHexData += *(pCyAsciiBuf + 1) - '0';
	}
	else if ('a' > *(pCyAsciiBuf + 1) ) //大写
	{
		cyHexData += *(pCyAsciiBuf + 1) - 'A' + 10;
	}
	else
	{
		cyHexData += *(pCyAsciiBuf + 1) - 'a' + 10;
	}
	
	return (cyHexData);
}

//**************************************************************************************************
// 名称         : MODBUS_ASCII_GetLrc()
// 创建日期     : 2015-07-24
// 作者         : 陈锦
// 功能         : 获取LRC值
// 输入参数     : ASCII码串(u8 *pCyAsciiBuf), 数据长度(u8 cyLen)
// 输出参数     : 无
// 返回结果     : LRC值(u8 cyLrcVal)
// 注意和说明   : 
// 修改内容     :
//**************************************************************************************************
uint8_t MODBUS_ASCII_GetLrc(uint8_t *pCyAsciiBuf, uint8_t cyLen)
{
	uint8_t i;
	uint8_t cyLrcVal;
	
	if (1 == (cyLen % 2) )
	{
		return 0;
	}
	
	cyLen /= 2;
	cyLrcVal = 0;
	for (i = 0; i < cyLen; i++)
	{
		cyLrcVal += MODBUS_ASCII_AsciiToHex(pCyAsciiBuf + i * 2);
	}
	//求补码
	cyLrcVal = ~cyLrcVal;
	cyLrcVal += 1;
	
	return (cyLrcVal);
}


//**************************************************************************************************
// 名称         : MODBUS_ASCII_AsciiPacketToRtuPacket()
// 创建日期     : 2015-07-24
// 作者         : 陈锦
// 功能         : ASCII数据包转成RTU数据包
// 输入参数     : ASCII码串(u8 *pCyAsciiBuf),  ASCII码串包长度(u8 cyAsciiLen)
// 输出参数     : RTU码串(u8 *pCyRtuBuf),
// 返回结果     : 0:错误；其他：RTU码串包长度(u8 cyRtuLen)
// 注意和说明   : 
// 修改内容     :
//**************************************************************************************************
uint8_t MODBUS_ASCII_AsciiPacketToRtuPacket(uint8_t *pCyAsciiBuf, uint8_t cyAsciiLen, uint8_t *pCyRtuBuf)
{
	uint8_t i;
	uint8_t cyRtuLen;
	
	if (1 == (cyAsciiLen % 2) )
	{
		return 0;
	}
	
	cyRtuLen = cyAsciiLen / 2;
	for (i = 0; i < cyRtuLen; i++)
	{
		*(pCyRtuBuf + i) = MODBUS_ASCII_AsciiToHex(pCyAsciiBuf + i * 2);
	}
	
	return (cyRtuLen);
}

//**************************************************************************************************
// 名称         : MODBUS_ASCII_RtuPacketToAsciiPacket()
// 创建日期     : 2015-07-24
// 作者         : 陈锦
// 功能         : RTU数据包转成ASCII数据包
// 输入参数     : RTU码串(u8 *pCyRtuBuf),  RTU码串包长度(u8 cyRtuLen)
// 输出参数     : ASCII码串(u8 *pCyAsciiBuf),
// 返回结果     : ASCII码串包长度(u8 cyAsciiLen)
// 注意和说明   : 
// 修改内容     :
//**************************************************************************************************

uint8_t MODBUS_ASCII_RtuPacketToAsciiPacket(uint8_t *pCyRtuBuf, uint8_t cyRtuLen, uint8_t *pCyAsciiBuf)
{
	uint8_t i;
	uint8_t cyAsciiLen;
	
	cyAsciiLen = cyRtuLen * 2;
	for (i = 0; i < cyRtuLen; i++)
	{
		MODBUS_ASCII_HexToAscii( *(pCyRtuBuf + i), pCyAsciiBuf + i * 2);
	}
	
	return (cyAsciiLen);
}


//**************************************************************************************************
// 名称         : MODBUS_ASCII_HandlRevData()
// 创建日期     : 2015-07-27
// 作者         : 陈锦
// 功能         : ASCII处理接收数据
// 输入参数     : 接收数据(u8 cyRevData)
// 输出参数     : 无
// 返回结果     : 无
// 注意和说明   :
// 修改内容     :
//**************************************************************************************************
//void MODBUS_ASCII_HandlRevData(uint8_t cyRevData)
//{  
//	uint8_t tempbuf[8]={0};
//    switch(g_cyRevState)
//    {//rt_kprintf("RevData_comin\n");
//        case ASCII_HEAD_STATE:  
//          if (ASCII_HEAD_DATA == cyRevData)
//          {
//              g_cyRevBufffLen = 0;
//          }
//          else if (0x0D == cyRevData)
//          {
//              g_cyRevState = ASCII_END_STATE;
//          }
//          UART1_RXBuff[g_cyRevBufffLen] = cyRevData;
//          g_cyRevBufffLen++;
//		//  rt_kprintf("ASCII_HEAD_STATE %s\n",UART1_RXBuff);
//          if (USART1_MAX_DATALEN <= g_cyRevBufffLen)
//          {
//              g_cyRevState = ASCII_IDLE_STATE;
//          }
//          break;
//          
//          case ASCII_END_STATE:   
//            if (ASCII_HEAD_DATA == cyRevData)
//            {
//                g_cyRevBufffLen = 0;
//                g_cyRevState = ASCII_HEAD_STATE;
//                UART1_RXBuff[g_cyRevBufffLen] = cyRevData;
//                g_cyRevBufffLen++;
//            }
//            else if(0x0A == cyRevData)
//            {
//                g_cyRevState = ASCII_IDLE_STATE;
//
//                UART1_RXBuff[g_cyRevBufffLen] = cyRevData;
//                g_cyRevBufffLen++;
//
//                UartRecvFrameOK = SET;                                          //发送成功接收一包数据的标志	
//                UartRecvFlag = RESET;                                           //接收完了
//				 //Swy_big_endian_cpy(tempbuf, UART1_RXBuff+4, 4);
//				 rt_memcpy(get_ds18b20_temp, UART1_RXBuff+11,4);
//				// get_temp_value= hextodec_fun(tempbuf); 
//				 rt_kprintf("asc_rec %s %d\n",get_ds18b20_temp,get_temp_value);
//                // rt_memcpy(tempbuf, UART1_RXBuff+11, 4);
//			   rt_kprintf("asc_rec_ok %s\n",UART1_RXBuff);
//				// rt_memset(UART1_RXBuff, 0, 200);
//				 
//			}
//            else
//            {
//                g_cyRevState = ASCII_IDLE_STATE;
//            }
//            break; 
//            
//            case ASCII_IDLE_STATE:  
//            if (ASCII_HEAD_DATA == cyRevData)
//            {
//                g_cyRevBufffLen = 0;
//                g_cyRevState = ASCII_HEAD_STATE;
//                UART1_RXBuff[g_cyRevBufffLen] = cyRevData;
//                g_cyRevBufffLen++;
//            }
//			
//			//rt_kprintf("ASCII_IDLE_STATE %s\n",UART1_RXBuff);
//            break;
//            
//           default:               
//             g_cyRevState = ASCII_IDLE_STATE; 
//           break;
//    }
//}
int32_t get_temperature(void)
{  
   int32_t result=0;
    unsigned char tempbuf[8]={0};
	//int	*numbuf;
  // Swy_big_endian_cpy(tempbuf, UART1_RXBuff+4, 4);
   rt_enter_critical();
   //rt_memcpy(tempbuf, UART1_RXBuff+7, 8);
   Swy_big_endian_cpy(tempbuf, UART1_RXBuff+7, 8);
   //result= hextodec_fun(tempbuf); 
   rt_kprintf("tempbuf %s\n",tempbuf);
   rt_kprintf("get_temperature %d\n",result);
   rt_exit_critical();
 //  rt_memset(tempbuf, 0, 8);
   return result;
}
void MODBUS_ASCII_HandlRevData(uint8_t cyRevData)
{  
	unsigned char tempbuf[8]={0}; 
    switch(g_cyRevState)
    {
        case ASCII_HEAD_STATE:  
          if (ASCII_HEAD_DATA == cyRevData)
          {
              g_cyRevBufffLen = 0;
          }
          else if (0x0D == cyRevData)
          {
              g_cyRevState = ASCII_END_STATE;
          }
          UART1_RXBuff[g_cyRevBufffLen] = cyRevData;
          g_cyRevBufffLen++;
		//  rt_kprintf("ASCII_HEAD_STATE %s\n",UART1_RXBuff);
          if (USART1_MAX_DATALEN <= g_cyRevBufffLen)
          {
              g_cyRevState = ASCII_IDLE_STATE;
          }
          break;
          
          case ASCII_END_STATE:   
            if (ASCII_HEAD_DATA == cyRevData)
            {
                g_cyRevBufffLen = 0;
                g_cyRevState = ASCII_HEAD_STATE;
                UART1_RXBuff[g_cyRevBufffLen] = cyRevData;
                g_cyRevBufffLen++;
            }
            else if(0x0A == cyRevData)
            {
                g_cyRevState = ASCII_IDLE_STATE;

                UART1_RXBuff[g_cyRevBufffLen] = cyRevData;
                g_cyRevBufffLen++;

                UartRecvFrameOK = SET;                                          //发送成功接收一包数据的标志	
                UartRecvFlag = RESET;                                           //接收完了

//							 rt_memcpy(&get_ds18b20_temp, UART1_RXBuff+5,2);
//							// get_temp_value= hextodec_fun(tempbuf); 
//							 rt_kprintf("asc_rec %s\n",get_ds18b20_temp);                
							if (0x32 == UART1_RXBuff[1] && 0x31 == UART1_RXBuff[2])
							{
								 rt_enter_critical();
								 Swy_big_endian_cpy(tempbuf, UART1_RXBuff+12, 3);
								 get_ds18b20_temp= hextodec_fun(tempbuf); 
								 rt_kprintf("tempbuf %d\n",get_ds18b20_temp);
								 RunVar.TempInAir = get_ds18b20_temp;
								 rt_exit_critical();
			//					rt_sem_release(temp_sem);
//								 rt_event_send(&temp_event, 2);
//			           rt_event_send(&nbtempsend_event, 2);                   
								
							}
					  	else
							{
								rt_kprintf("cap_recv:%s",UART1_RXBuff);
					      //rt_sem_release(cap_sem);
//								rt_event_send(&cap_event, 1);
							}
				 
			      }
            else
            {
                g_cyRevState = ASCII_IDLE_STATE;
            }
            break; 
            
            case ASCII_IDLE_STATE:  
            if (ASCII_HEAD_DATA == cyRevData)
            {
                g_cyRevBufffLen = 0;
                g_cyRevState = ASCII_HEAD_STATE;
                UART1_RXBuff[g_cyRevBufffLen] = cyRevData;
                g_cyRevBufffLen++;
            }
			
			//rt_kprintf("ASCII_IDLE_STATE %s\n",UART1_RXBuff);
            break;
            
           default:               
             g_cyRevState = ASCII_IDLE_STATE; 
           break;
    }
}

void MODBUS_ASCII_HandlRevData_C(uint8_t cyRevData)
{
   // OS_ERR  os_err;

    if(g_cyRevBufffLen > USART1_MAX_DATALEN)
    {
      //UsartErrCnt = 30;
      g_cyRevBufffLen = 0;
      //USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
      //USART_Cmd(USART1, DISABLE);
      return;
    }
 //   rt_kprintf("%x ",cyRevData);
    if(0x3A == cyRevData)
    {
      g_cyRevBufffLen = 0;
      UART1_RXBuff[g_cyRevBufffLen++] = cyRevData;
    }
    else if((0x0A == cyRevData)&&(3 < g_cyRevBufffLen))
    { 
      if(0x0D == UART1_RXBuff[g_cyRevBufffLen - 1])
      {
        UART1_RXBuff[g_cyRevBufffLen++] = cyRevData;
        
        UartRecvFrameOK = SET;
     //   #if defined PKG_USING_DS18B20
				if (0x32 == UART1_RXBuff[1] && 0x31 == UART1_RXBuff[2])
				{
					rt_memcpy(&get_ds18b20_temp, UART1_RXBuff+11,4);
//					rt_sem_release(temp_sem);
//					 rt_event_send(&temp_event, 2);
          rt_kprintf("temp_recv1 %s",UART1_RXBuff);

//					rt_event_recv(&event, UART_RX_EVENT,RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,RT_WAITING_FOREVER, &e);
				}
			//	#endif
			else
				{
					rt_kprintf("cap_recv2:%s",UART1_RXBuff);
		      //rt_sem_release(cap_sem);
//					rt_event_send(&cap_event, UART_RX_EVENT);
				}

			}
    }
    else
    {
      UART1_RXBuff[g_cyRevBufffLen++] = cyRevData;
			
    }
    
}

//**************************************************************************************************
// 名称         : MODBUS_ASCII_CheckAscii()
// 创建日期     : 2015-07-27
// 作者         : 陈锦
// 功能         : 检验是否都是Ascii码
// 输入参数     : ASCII码串(u8 *pCyAsciiBuf), 数据长度(u8 cyLen)
// 输出参数     : 无
// 返回结果     : 检测(0 不全是， 1 全是)
// 注意和说明   : 
// 修改内容     :
//**************************************************************************************************
uint8_t MODBUS_ASCII_CheckAscii(uint8_t *pCyAsciiBuf, uint8_t cyLen)
{
	uint8_t i;
	
	for (i = 0; i < cyLen; i++)
	{
		if ('0' > *(pCyAsciiBuf + i) )
		{
			break;
		}
		
		if ( ('9' < *(pCyAsciiBuf + i) ) && ( *(pCyAsciiBuf + i) < 'A' ) )
		{
			break;
		}
		
		if ( ('F' < *(pCyAsciiBuf + i) ) && ( *(pCyAsciiBuf + i) < 'a' ) )
		{
			break;
		}
		
		if ('f' < *(pCyAsciiBuf + i) )
		{
			break;
		}
	}
	
	
	if (i == cyLen)
	{
		return (1);
	}
	
	return (0);
}

//**************************************************************************************************
// 名称         : MODBUS_ASCII_RecvData()
// 创建日期     : 2015-07-27
// 作者         : 陈锦
// 功能         : 接收一串数据
// 输入参数     : 数据串(cyRecvBuff)
// 输出参数     : 数据长度(u8 *cyLen)
// 返回结果     : 执行结果(0 没有接收数据， 1 接收数据出现不是ASCII码， 2 效验码错误， 3 成功)
// 注意和说明   :
// 修改内容     :
//**************************************************************************************************

uint8_t MODBUS_ASCII_RecvData(uint8_t* cyRecvBuff, uint8_t *pCyLen)
{
    uint8_t cyLrc;
	
    if (((uint8_t*)NULL) == cyRecvBuff)
    {  rt_kprintf("modRecvData NULL\n");
        return 0;
    }

    if ((Bit_RESET == UartRecvFrameOK) || (0 == g_cyRevBufffLen))
    {rt_kprintf("modRecvData2 NULL\n");
        return 0;
    }
    
    UartRecvFrameOK = RESET;
    
    if (0 == MODBUS_ASCII_CheckAscii(&UART1_RXBuff[1], g_cyRevBufffLen - 3) )
    {
    rt_kprintf("发送数据错误\n");
    	//发送数据错误
    	return 1;
    }

    cyLrc = MODBUS_ASCII_GetLrc(&UART1_RXBuff[1], g_cyRevBufffLen - 5);
		if (cyLrc != MODBUS_ASCII_AsciiToHex(&UART1_RXBuff[g_cyRevBufffLen - 4]) )
    { rt_kprintf("发送数据错误2\n");
    	//发送数据效验错误
    	return 2;
    }

    *pCyLen = MODBUS_ASCII_AsciiPacketToRtuPacket(&UART1_RXBuff[1], g_cyRevBufffLen - 5, cyRecvBuff);
	
    return (3);
}


//**************************************************************************************************
// 名称         : MODBUS_ASCII_SendData()
// 创建日期     : 2015-07-13
// 作者         : 陈锦
// 功能         : 发送一串数据
// 输入参数     : 数据串(u8 *cySendBuff), 数据长度(cyLen) (数据长度 小于 123)
// 输出参数     : 无
// 返回结果     : 执行结果(0 失败， 数据长度 成功)
// 注意和说明   :
// 修改内容     :
//**************************************************************************************************
uint8_t MODBUS_ASCII_SendData(uint8_t *cySendBuff, uint8_t cyLen)
{
    uint8_t cyLrc;
    uint8_t cyAsciiLen;
	
    if ( (0 == cyLen) || ( ((uint8_t*)NULL) == cySendBuff) )
    {
        return 0;
    }
    
    if ( (cyLen * 2 + 5) > USART1_MAX_DATALEN)
    {
    	return 0;
    }
    
    cyAsciiBuffc[0] = ASCII_HEAD_DATA;
    cyAsciiLen = 1;
    
    cyAsciiLen += MODBUS_ASCII_RtuPacketToAsciiPacket(cySendBuff, cyLen, &cyAsciiBuffc[1]);
    cyLrc = MODBUS_ASCII_GetLrc(&cyAsciiBuffc[1], cyAsciiLen - 1);
    MODBUS_ASCII_HexToAscii(cyLrc, &cyAsciiBuffc[cyAsciiLen]);
    cyAsciiLen += 2;
    cyAsciiBuffc[cyAsciiLen] = 0x0D;
    cyAsciiLen++;
    cyAsciiBuffc[cyAsciiLen] = 0x0A;
    cyAsciiLen++;
    
//		rt_kprintf("sendasci:%s",cyAsciiBuffc);
    return ( UART1_SendData(cyAsciiBuffc, cyAsciiLen) );
}
