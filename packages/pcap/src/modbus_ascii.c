#include "bsp.h"
//#include "uffs_types.h"
extern uint32_t get_ds18b20_temp;

extern FlagStatus UartRecvFlag;
extern FlagStatus UartRecvFrameOK ;
extern uint8_t UART1_TXBuff[USART1_MAX_DATALEN];
extern uint8_t UART1_RXBuff[USART1_MAX_DATALEN];     //���ջ���,���USART_REC_LEN���ֽ�.
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
** ��Ȩ��   2015-2025, ��������Ϊ�Ƽ���չ���޹�˾
** �ļ���:  Modbus_Ascii.h
** ����:    �½�
** �汾:    V1.0.0
** ����:    2015-07-10
** ����:    �����㷨
** ����:         
*************************************************************************************************************************************************************************
** �޸���:          No
** �汾:  		
** �޸�����:    No 
** ����:            No
*************************************************************************************************************************************************************************/
//**************************************************************************************************
// ����         : MODBUS_ASCII_HexToAscii()
// ��������     : 2015-07-24
// ����         : �½�
// ����         : ʮ��������תASCII��
// �������     : ʮ��������(u8 cyHexData)
// �������     : ASCII��(u8 *pCyAsciiBuf)
// ���ؽ��     : ��
// ע���˵��   : 
// �޸�����     :
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
// ����         : MODBUS_ASCII_AsciiToHex()
// ��������     : 2015-07-24
// ����         : �½�
// ����         : ASCII��תʮ��������
// �������     : ASCII��(u8 *pCyAsciiBuf)
// �������     : ��
// ���ؽ��     : ʮ��������(u8 cyHexData)
// ע���˵��   : 
// �޸�����     :
//**************************************************************************************************
uint8_t MODBUS_ASCII_AsciiToHex(uint8_t *pCyAsciiBuf)
{
	uint8_t cyHexData;
	
	cyHexData = 0;
	if ('A' > *(pCyAsciiBuf + 0) ) //0-9
	{
		cyHexData += *(pCyAsciiBuf + 0) - '0';
	}
	else if ('a' > *(pCyAsciiBuf + 0) ) //��д
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
	else if ('a' > *(pCyAsciiBuf + 1) ) //��д
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
// ����         : MODBUS_ASCII_GetLrc()
// ��������     : 2015-07-24
// ����         : �½�
// ����         : ��ȡLRCֵ
// �������     : ASCII�봮(u8 *pCyAsciiBuf), ���ݳ���(u8 cyLen)
// �������     : ��
// ���ؽ��     : LRCֵ(u8 cyLrcVal)
// ע���˵��   : 
// �޸�����     :
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
	//����
	cyLrcVal = ~cyLrcVal;
	cyLrcVal += 1;
	
	return (cyLrcVal);
}


//**************************************************************************************************
// ����         : MODBUS_ASCII_AsciiPacketToRtuPacket()
// ��������     : 2015-07-24
// ����         : �½�
// ����         : ASCII���ݰ�ת��RTU���ݰ�
// �������     : ASCII�봮(u8 *pCyAsciiBuf),  ASCII�봮������(u8 cyAsciiLen)
// �������     : RTU�봮(u8 *pCyRtuBuf),
// ���ؽ��     : 0:����������RTU�봮������(u8 cyRtuLen)
// ע���˵��   : 
// �޸�����     :
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
// ����         : MODBUS_ASCII_RtuPacketToAsciiPacket()
// ��������     : 2015-07-24
// ����         : �½�
// ����         : RTU���ݰ�ת��ASCII���ݰ�
// �������     : RTU�봮(u8 *pCyRtuBuf),  RTU�봮������(u8 cyRtuLen)
// �������     : ASCII�봮(u8 *pCyAsciiBuf),
// ���ؽ��     : ASCII�봮������(u8 cyAsciiLen)
// ע���˵��   : 
// �޸�����     :
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
// ����         : MODBUS_ASCII_HandlRevData()
// ��������     : 2015-07-27
// ����         : �½�
// ����         : ASCII�����������
// �������     : ��������(u8 cyRevData)
// �������     : ��
// ���ؽ��     : ��
// ע���˵��   :
// �޸�����     :
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
//                UartRecvFrameOK = SET;                                          //���ͳɹ�����һ�����ݵı�־	
//                UartRecvFlag = RESET;                                           //��������
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

                UartRecvFrameOK = SET;                                          //���ͳɹ�����һ�����ݵı�־	
                UartRecvFlag = RESET;                                           //��������

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
// ����         : MODBUS_ASCII_CheckAscii()
// ��������     : 2015-07-27
// ����         : �½�
// ����         : �����Ƿ���Ascii��
// �������     : ASCII�봮(u8 *pCyAsciiBuf), ���ݳ���(u8 cyLen)
// �������     : ��
// ���ؽ��     : ���(0 ��ȫ�ǣ� 1 ȫ��)
// ע���˵��   : 
// �޸�����     :
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
// ����         : MODBUS_ASCII_RecvData()
// ��������     : 2015-07-27
// ����         : �½�
// ����         : ����һ������
// �������     : ���ݴ�(cyRecvBuff)
// �������     : ���ݳ���(u8 *cyLen)
// ���ؽ��     : ִ�н��(0 û�н������ݣ� 1 �������ݳ��ֲ���ASCII�룬 2 Ч������� 3 �ɹ�)
// ע���˵��   :
// �޸�����     :
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
    rt_kprintf("�������ݴ���\n");
    	//�������ݴ���
    	return 1;
    }

    cyLrc = MODBUS_ASCII_GetLrc(&UART1_RXBuff[1], g_cyRevBufffLen - 5);
		if (cyLrc != MODBUS_ASCII_AsciiToHex(&UART1_RXBuff[g_cyRevBufffLen - 4]) )
    { rt_kprintf("�������ݴ���2\n");
    	//��������Ч�����
    	return 2;
    }

    *pCyLen = MODBUS_ASCII_AsciiPacketToRtuPacket(&UART1_RXBuff[1], g_cyRevBufffLen - 5, cyRecvBuff);
	
    return (3);
}


//**************************************************************************************************
// ����         : MODBUS_ASCII_SendData()
// ��������     : 2015-07-13
// ����         : �½�
// ����         : ����һ������
// �������     : ���ݴ�(u8 *cySendBuff), ���ݳ���(cyLen) (���ݳ��� С�� 123)
// �������     : ��
// ���ؽ��     : ִ�н��(0 ʧ�ܣ� ���ݳ��� �ɹ�)
// ע���˵��   :
// �޸�����     :
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
