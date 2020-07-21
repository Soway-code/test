/**@file        modbus_ascii.c
* @brief        ModBus ASCII数据处理
* @details      ModBus ASCII数据与ModBus RTU数据互转
* @author       杨春林
* @date         2020-06-05
* @version      V1.0.1
* @copyright    2020-2030,深圳市信为科技发展有限公司
**********************************************************************************
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/06/05  <td>1.0.1    <td>杨春林    <td>对MODBUS_ASCII_SendData函数添加了等待串口DMA\n
* 发送完毕的程序, 确保上一次数据发送完毕再发送本次的数据
* </table>
*
**********************************************************************************
*/

#include "modbus_ascii.h"
#include "usart_app.h"


static uint8_t Send_Buf[USART_ASCII_MIN_DATALEN];   ///< 串口发送数据缓存


/**@brief       十六进制数转ASCII码
* @param[in]    cyHexData : 十六进制数
* @param[out]   pCyAsciiBuf : ASCII码
* @return       函数执行结果
* - None
*/
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

/**@brief       ASCII码转十六进制数
* @param[in]    pCyAsciiBuf : ASCII码
* @return       函数执行结果
* - 十六进制数
*/
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

/**@brief       获取LRC值
* @param[in]    RtuBuf : RTU数据缓存指针
* @param[in]    cyLen : 数据长度
* @return       函数执行结果
* - LRC值
*/
uint8_t MODBUS_GetLrc(uint8_t *pCyRtuBuf, uint16_t cyLen)
{
	int8_t cyLrcVal;
    uint16_t i;
	
	cyLrcVal = 0;
	for (i = 0; i < cyLen; i++)
	{
		cyLrcVal += pCyRtuBuf[i];
	}
	//求补码
	cyLrcVal = -cyLrcVal;
	
	return (uint8_t)cyLrcVal;
}

/**@brief       ASCII数据包转成RTU数据包
* @param[in]    pCyAsciiBuf : ASCII数据包指针
* @param[in]    cyAsciiLen : ASCII数据包长度
* @param[out]   pCyRtuBuf : RTU数据包指针
* @return       函数执行结果
* - 错误 : 0
* - 成功 : RTU数据包长度
*/
uint16_t MODBUS_ASCII_AsciiPacketToRtuPacket(uint8_t *pCyAsciiBuf, uint16_t cyAsciiLen, uint8_t *pCyRtuBuf)
{
	uint16_t i;
	uint16_t cyRtuLen;
	
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

/**@brief       RTU数据包转成ASCII数据包
* @param[in]    pCyRtuBuf : RTU数据包指针
* @param[in]    cyRtuLen : RTU数据包长度
* @param[out]   pCyAsciiBuf : ASCII数据包指针
* @return       函数执行结果
* - ASCII数据包长度
*/
uint16_t MODBUS_ASCII_RtuPacketToAsciiPacket(uint8_t *pCyRtuBuf, uint16_t cyRtuLen, uint8_t *pCyAsciiBuf)
{
	uint16_t i;
	uint16_t cyAsciiLen;
	
	cyAsciiLen = cyRtuLen * 2;
	for (i = 0; i < cyRtuLen; i++)
	{
		MODBUS_ASCII_HexToAscii( *(pCyRtuBuf + i), pCyAsciiBuf + i * 2);
	}
	
	return (cyAsciiLen);
}

/**@brief       检验是否都是Ascii码
* @param[in]    pCyAsciiBuf : ASCII数据包指针
* @param[in]    cyLen : ASCII数据包长度
* @return       函数执行结果
* - ERR_NONE : 无错误
* - ERR_ASCII : 不全是ASCII码
*/
uint8_t MODBUS_ASCII_CheckAscii(uint8_t *pCyAsciiBuf, uint16_t cyLen)
{
	uint16_t i;
	
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
		return ERR_NONE;
	}
	
	return ERR_ASCII;
}

/**@brief       获取一包数据
* @param[in]    cyRecvBuff : 接收缓存指针
* @param[in]    cyLen : 接收的数据长度
* @return       函数执行结果
* - ERR_INVALID_DATA : 接收缓存指针无效
* - ERR_ASCII : 不全是ASCII码
* - ERR_LRC : LRC校验错误
* - ERR_NONE : 无错误
*/
uint8_t MODBUS_ASCII_RecvData(uint8_t* cyRecvBuff, uint16_t *pCyLen)
{
    uint8_t cyLrc;
    uint16_t recv_len;
	
    if (((uint8_t*)0) == cyRecvBuff)
    {
        return ERR_INVALID_DATA;
    }
    recv_len = Sensor_USART_GetReceData(cyRecvBuff);
    
    if((recv_len - 2) <= 0
        || cyRecvBuff[0] != ASCII_HEAD_DATA
        || cyRecvBuff[recv_len - 2] != ASCII_TAIL_FIRST_DATA)
    {
        return ERR_INVALID_DATA;
    }
    
    if (ERR_ASCII == MODBUS_ASCII_CheckAscii(&cyRecvBuff[1], recv_len - 3) )
    {
    	return ERR_ASCII;        
    }
    
    *pCyLen = MODBUS_ASCII_AsciiPacketToRtuPacket(&cyRecvBuff[1], 
            recv_len - 3, cyRecvBuff);
    
    cyLrc = MODBUS_GetLrc(cyRecvBuff, *pCyLen - 1);
    if (cyLrc != cyRecvBuff[*pCyLen - 1])
    {
    	return ERR_LRC;
    }        
    *pCyLen -= 1;
	
    return ERR_NONE;
}

/**@brief       发送一包数据
* @param[in]    cySendBuff : 发送缓存指针
* @param[in]    cyLen : 发送的数据长度
* @return       函数执行结果
* - OP_SUCCESS(成功)
* - OP_FAILED(失败)
*/
uint8_t MODBUS_ASCII_SendData(uint8_t *cySendBuff, uint16_t cyLen)
{
    uint8_t result;
    uint8_t cyLrc;
    uint16_t cyAsciiLen;
	
    if ( (0 == cyLen) || ( ((uint8_t*)0) == cySendBuff) )
    {
        return 0;
    }
    
    if ( (cyLen * 2 + 5) > USART_ASCII_MAX_DATALEN)
    {
    	return 0;
    }
    
    Send_Buf[0] = ASCII_HEAD_DATA;
    cyAsciiLen = 1;
    
    cyLrc = MODBUS_GetLrc(cySendBuff, cyLen);
    cyAsciiLen += MODBUS_ASCII_RtuPacketToAsciiPacket(cySendBuff, cyLen, &Send_Buf[1]);    
    MODBUS_ASCII_HexToAscii(cyLrc, &Send_Buf[cyAsciiLen]);
    cyAsciiLen += 2;
    Send_Buf[cyAsciiLen] = 0x0D;
    cyAsciiLen++;
    Send_Buf[cyAsciiLen] = 0x0A;
    cyAsciiLen++;
    
    while(Sensor_USART_Get_TX_Cplt_Flag() == 0);
    Sensor_USART_Clear_TX_Cplt_Flag();
#ifdef  USART_USING_485
    TX_ON;
#endif
    result = BSP_UART_Transmit_DMA(Send_Buf, cyAsciiLen);
    
    return result;
}

