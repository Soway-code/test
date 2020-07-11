#include "check.h"

extern FlashPara_Typedef	  FlashPara;										// Flash��������ṹ��

uint8_t MDB_Check(uint8_t *master_request_data, uint8_t len)    				// ������ϢУ��
{
    uint8_t CheckResult = CHECK_NO_ERROR;

    if(FlashPara.Customer.OutPutMod == OUTPUTMOD_RTU)                 			// RTUģʽʹ��CRCУ�鷽ʽ
    {
        if(MDB_Get_CRC16(master_request_data, len))             				// У������Ϊ0��˵��У�����
            CheckResult = MDB_MSG_FRAME_CHECK_ERROR;
    }
    
    else
    {
        if(FlashPara.Customer.OutPutMod == OUTPUTMOD_ASCII)           			// ASCIIģʽʹ��LRCУ�鷽ʽ
		{	
			if(MDB_Get_LRC(master_request_data, (len - 3) / 2))					// LRCУ�� ��ȥ��1����ʼ�������2�ֽڽ����� ASCIIģʽÿ2�ֽڱ�ʾ1�ֽ�����
				CheckResult = MDB_MSG_FRAME_CHECK_ERROR;
		}
    }
    
    return CheckResult;
}

//////////////////////////////////////////////////////////////////////////////////////////
//description������MDB_Get_CRC16() ��ȡ�ַ����е�CRCУ����
//input����      ��Msg��MsgLenʮ�������ַ����м��䳤��
//output���     ��16λCRC��
///////////////////////////////////////////////////////////////////////////////////////////
uint16_t MDB_Get_CRC16(uint8_t *Msg, uint8_t MsgLen)
{
    uint8_t CRCLo = 0xFF ;                                      				// ��CRC�ֽڳ�ʼ��
	uint8_t CRCHi = 0xFF ;                                      				// ��CRC�ֽڳ�ʼ�� 
	uint8_t uIndex;                                          	  				// CRCѭ���е����� 
    
	while (MsgLen--)                                            				// ������Ϣ������ 
	{ 
		uIndex = CRCLo ^ *Msg++;                                				// ����CRC 
		CRCLo  = CRCHi ^ auchCRCLo[uIndex]; 
		CRCHi  = auchCRCHi[uIndex]; 
	}
    
	return (CRCHi << 8) | CRCLo;
}

uint8_t MDB_Get_LRC(uint8_t *Msg, uint8_t MsgLen)              					// LRCУ��
{
    uint8_t i = 0;
    uint8_t sum = 0;

    for(i = 0; i < MsgLen; i++)
        sum = sum + Msg[i];

    return 256 - sum;                                                 			// 
}

