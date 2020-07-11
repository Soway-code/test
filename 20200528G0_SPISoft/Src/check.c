#include "check.h"

extern FlashPara_Typedef	  FlashPara;										// Flash保存参数结构体

uint8_t MDB_Check(uint8_t *master_request_data, uint8_t len)    				// 主机消息校验
{
    uint8_t CheckResult = CHECK_NO_ERROR;

    if(FlashPara.Customer.OutPutMod == OUTPUTMOD_RTU)                 			// RTU模式使用CRC校验方式
    {
        if(MDB_Get_CRC16(master_request_data, len))             				// 校验结果不为0则说明校验错误
            CheckResult = MDB_MSG_FRAME_CHECK_ERROR;
    }
    
    else
    {
        if(FlashPara.Customer.OutPutMod == OUTPUTMOD_ASCII)           			// ASCII模式使用LRC校验方式
		{	
			if(MDB_Get_LRC(master_request_data, (len - 3) / 2))					// LRC校验 除去第1个起始符和最后2字节结束符 ASCII模式每2字节表示1字节数据
				CheckResult = MDB_MSG_FRAME_CHECK_ERROR;
		}
    }
    
    return CheckResult;
}

//////////////////////////////////////////////////////////////////////////////////////////
//description描述：MDB_Get_CRC16() 获取字符序列的CRC校验码
//input输入      ：Msg，MsgLen十六进制字符序列及其长度
//output输出     ：16位CRC码
///////////////////////////////////////////////////////////////////////////////////////////
uint16_t MDB_Get_CRC16(uint8_t *Msg, uint8_t MsgLen)
{
    uint8_t CRCLo = 0xFF ;                                      				// 高CRC字节初始化
	uint8_t CRCHi = 0xFF ;                                      				// 低CRC字节初始化 
	uint8_t uIndex;                                          	  				// CRC循环中的索引 
    
	while (MsgLen--)                                            				// 传输消息缓冲区 
	{ 
		uIndex = CRCLo ^ *Msg++;                                				// 计算CRC 
		CRCLo  = CRCHi ^ auchCRCLo[uIndex]; 
		CRCHi  = auchCRCHi[uIndex]; 
	}
    
	return (CRCHi << 8) | CRCLo;
}

uint8_t MDB_Get_LRC(uint8_t *Msg, uint8_t MsgLen)              					// LRC校验
{
    uint8_t i = 0;
    uint8_t sum = 0;

    for(i = 0; i < MsgLen; i++)
        sum = sum + Msg[i];

    return 256 - sum;                                                 			// 
}

