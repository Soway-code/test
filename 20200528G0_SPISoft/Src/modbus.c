
#include "modbus.h"

uint8_t MDBWriteFlag = MDBWriteNone;											// 该变量记录写输出模式 写时标记为1(MDBWriteOutPutMod)
uint8_t slave_response_data_len = 0;                                        	// 从机响应消息帧长度
uint8_t slave_response_data[DataPacketLen] = {0};                           	// 从机响应消息帧缓存

extern uint8_t RcvPacketLen;													// 接收到主机消息帧长度
extern uint16_t ParityErrorFlag;												// 串口奇偶校验错误标志
extern uint16_t Out_Put_Mode;
extern uint16_t SamplingFlag;													// 采样完成标志 当采样完成后 在TIM4中断中会标记为完成
extern UART_HandleTypeDef hlpuart1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern uint8_t Tim2Flag;
extern CustomerPara_Typedef Default_CustomerPara;
extern FlashPara_Typedef	FlashPara;											// Flash保存参数结构体
extern uint8_t master_request_data[DataPacketLen];
//extern PosSampling_Typedef  PosSampling;										// 位移采样参数结构体
//extern TempSampling_Typedef TempSampling;										// 温度采样参数结构体
//extern CalPara_Typedef		CalPara;											// 标定计算参数结构体
//extern TestPara_Typedef	  	Test;												// 内测数据参数结构体

// 获取从机地址
uint8_t MDB_Get_Slave_Addr(uint8_t *master_request_data)
{
	return *master_request_data;
}

// 功能操作函数
uint8_t MDB_Function(void)
{
	
    uint8_t err = MDB_NO_ERROR;                                                 // 错误编号    
    uint8_t MasterRequestFunction = 0;                                       	// 主机请求功能号

    MasterRequestFunction = master_request_data[1];
    
    slave_response_data[0] = master_request_data[0];							// 从机响应地址为本机地址或广播地址
    slave_response_data[1] = master_request_data[1];							// 从机响应功能号
	
	if(master_request_data[0] == BROADCAST_ADDR_0x00)							// 广播读地址
		MDB_F03_Broadcast_Read_Slave_Addr();
	
	else
    switch(MasterRequestFunction)
    {
        case MDB_F03_READ_HOLD_REG:                                             // 读保持寄存器数据
            err = MDB_F03_Read_Hold_Reg();
            break;
            
        case MDB_F04_READ_INPUT_REG:											// 读输入寄存器数据
            err = MDB_F04_Read_Input_Reg();
            break;
            
        case MDB_F05_EXT_WRITE_COIL_REG:
            err = MDB_F05_Write_Coil_Reg();
            break;
        
        case MDB_F10_WRITE_INPUT_REG:
            err = MDB_F10_Write_Hold_Reg();
            break;
            
        case MDB_F25_INT_WRITE_COIL_REG:
            err = MDB_F25_Int_Write_Coil_Reg();
            break;
		
        case MDB_F27_INT_WRITE_HOLD_REG:
			err = MDB_F27_Int_Write_Hold_Reg();
            break;
 
        default:
            err = MDB_FNO_INVALID;
            break;
    }
    
    if(err != MDB_NO_ERROR)                                                     // 主机请求帧有错误
    {
        slave_response_data[1] |= 0x80;
        slave_response_data[2]  = err;
        slave_response_data_len = 3;
    }

	MDB_Snd_Msg(slave_response_data, slave_response_data_len);					// 发送消息

	return err;
}

uint8_t FrameCheck(void)														// 串口帧校验检测
{
	uint8_t err = MDB_NO_ERROR;
	
	if(ParityErrorFlag)															// 奇偶校验错误或串口帧校验错误则直接返回 不作应答
	{
		ParityErrorFlag = 0;
		err = MDB_MSG_PARITY_ERROR;
		return err;
	}
	
	if((FlashPara.Customer.OutPutMod == OUTPUTMOD_RTU && RcvPacketLen < 5) || 
	   (FlashPara.Customer.OutPutMod == OUTPUTMOD_ASCII && RcvPacketLen < 9))	// 帧长度错误
	{
		err = MDB_MSG_LEN_ERROR;
		return err;
	}
	
	if(FlashPara.Customer.OutPutMod == OUTPUTMOD_ASCII)							// 通信为ASCII模式
		DataTransform();														// 将ASCII模式接收的数据转换成RTU模式 去掉了帧头和帧尾

	if(MDB_Check(master_request_data, RcvPacketLen))							// CRC/LRC校验
	{
		err = MDB_MSG_FRAME_CHECK_ERROR;
		return err;
	}
		
	if(MDB_Get_Slave_Addr(master_request_data) != FlashPara.Customer.Addr && 	// 非本机地址或广播地址
	   MDB_Get_Slave_Addr(master_request_data) != BROADCAST_ADDR_0x00)
	{
		err = MDB_MSG_ADDR_ERROR;
		return err;
	}																

	return err;	
}

uint8_t MDB_F03_Broadcast_Read_Slave_Addr(void)									// 广播读地址
{
	uint8_t  err = MDB_NO_ERROR;
	uint16_t ReadAddr = 0;
	uint16_t ReadNum = 0;
	
	ReadAddr = (master_request_data[2] << 8) + master_request_data[3];			// 读寄存器的地址值
	ReadNum = (master_request_data[4] << 8) + master_request_data[5];			// 读寄存器的个数

	if(master_request_data[1] != MDB_F03_READ_HOLD_REG)							// 广播读地址仅能使用03功能码
	{
		err = MDB_FNO_INVALID;													// 非支持的功能码
		return err;
	}
	
	if(ReadAddr != SLAVE_ADDR_0x30 || ReadNum != 1)								// 地址值不是从机地址寄存器地址值 或读取寄存器个数不为1
	{
		err = MDB_REG_ADDR_INVALID;												// 寄存器地址超出范围
		return err;
	}
	
	slave_response_data[2] = 0x02;												// 数据字节数
	slave_response_data[3] = FlashPara.Customer.Addr >> 8;						// 地址高8位
	slave_response_data[4] = FlashPara.Customer.Addr;							// 地址低8位
	slave_response_data_len = 5;
	
	return err;
}

uint8_t MDB_F03_Read_Hold_Reg(void)												// 读保持寄存器
{
    uint8_t  err = MDB_NO_ERROR;
    uint16_t ReadHoldRegAddr = 0;                                               // 读保持寄存器地址值

    ReadHoldRegAddr = (master_request_data[2] << 8) + master_request_data[3];
    
    switch(ReadHoldRegAddr)
    {
        case SLAVE_ADDR_0x30:
        case BAUD_ADDR_0x31:  
        case PARITY_ADDR_0x32:                                                  // 读从机地址 波特率 奇偶校验单个或多个数据 使用同一个功能函数
            err = MDB_F03_Read_Addr_Baud_Parity();
            break;
            
      
		case FREQUENCY_ADDR_0x3F:												// 读采样频率寄存器
        case OUT_PUT_MOD_ADDR_0x60:												// 读输出模式寄存器
            err = MDB_F03_Read_Single_Hold_Reg();
            break;
    
        default:
            err = MDB_REG_ADDR_INVALID;                                         // 寄存器地址不正确
            break;
    }
    
    return err;
}

uint8_t MDB_F03_Read_Addr_Baud_Parity(void)										// 读从机地址 波特率 奇偶校验单个或多个连续数据
{
    uint8_t  i = 0;
    uint8_t  err = MDB_NO_ERROR;
    uint16_t ReadRegNum = 0;                                                    // 读寄存器数量
    uint16_t ReadRegStartAddr = 0;                                              // 读寄存器起始地址
    uint16_t ReadRegEndAddr = 0;                                                // 读寄存器结束地址
    uint16_t RegData[3] = {0};
    
    RegData[0] = FlashPara.Customer.Addr;                                       // 将要读的参数存放在数组里 方便操作
    RegData[1] = FlashPara.Customer.Baud;
    RegData[2] = FlashPara.Customer.Parity;
    
    ReadRegNum = (master_request_data[4] << 8) + master_request_data[5];  
    ReadRegStartAddr = (master_request_data[2] << 8) + master_request_data[3];
    ReadRegEndAddr = ReadRegStartAddr + ReadRegNum - 1;

    if(ReadRegEndAddr > PARITY_ADDR_0x32 || ReadRegNum > 3)						// 地址超出范围
    {
        err = MDB_REG_ADDR_INVALID;												            // 寄存器值超出范围
        return err;
    }
    
    else
    {
        slave_response_data[2] = 2 * ReadRegNum;                                // 上传数据字节数
        for(i = 0; i < ReadRegNum; i++)
        {
            slave_response_data[3 + 2 * i] = RegData[ReadRegStartAddr - 0x30 + i] >> 8;     // 数据高8位
            slave_response_data[4 + 2 * i] = RegData[ReadRegStartAddr - 0x30 + i];          // 数据低8位
        }
        slave_response_data_len = 3 + 2 * ReadRegNum;                           // 
    }
     
    return err;
}

uint8_t MDB_F03_Read_Single_Hold_Reg(void)										// 读补偿使能数据
{
    uint8_t  err = MDB_NO_ERROR;
    uint16_t ReadRegNum = 0;
	uint16_t ReadRegAddr = 0;

	ReadRegAddr = (master_request_data[2] << 8) + master_request_data[3];		// 读寄存器地址
    ReadRegNum = (master_request_data[4] << 8) + master_request_data[5];		// 读寄存器数量
    
	if(ReadRegNum != 1)                                                         // 读单个寄存时 数值为1
    {
        err = MDB_REG_ADDR_INVALID;												// 寄存器值超出范围
        return err;
    }
    
    else
    {
        slave_response_data[2] = 0x02;                                          // 上传数据字节数 单个寄存器为16位2个字节
		

		
		if(ReadRegAddr == FREQUENCY_ADDR_0x3F)
		{
			slave_response_data[3] = FlashPara.Customer.UploadFreq >> 8;			// 数据高8位
        	slave_response_data[4] = FlashPara.Customer.UploadFreq;				// 数据低8位
		}
		
		if(ReadRegAddr == OUT_PUT_MOD_ADDR_0x60)
		{
			slave_response_data[3] = FlashPara.Customer.OutPutMod >> 8;			// 数据高8位
        	slave_response_data[4] = FlashPara.Customer.OutPutMod;				// 数据低8位
		}
        
        slave_response_data_len = 5;
    }
    
    return err;
}

uint8_t MDB_F04_Read_Input_Reg(void)											// 读输入寄存器 位移或温度 ReadRegNum为2时读单个 为4时两个同时读两个
{
    uint8_t  err = MDB_NO_ERROR;
    uint16_t ReadRegNum = 0;
    uint16_t ReadRegAddr = 0;
    
    ReadRegNum = (master_request_data[4] << 8) + master_request_data[5];  
    ReadRegAddr = (master_request_data[2] << 8) + master_request_data[3];
    
    switch(ReadRegAddr)
    {  
        case ACC_ADDR_0x02:
            if(ReadRegNum == 2)                                                 // 读温度
            {
                slave_response_data[2] = 0x04;                                  // 上传数据字节数 输入寄存器为32位4个字节
                slave_response_data[3] = 0x00;
                slave_response_data[4] = datax[0];
                slave_response_data[5] = datax[1];
                slave_response_data[6] = datax[2]&0xF0;
                slave_response_data_len = 7;
            }
			
			else
            {
                if(ReadRegNum == 4)                                             // 读温度和位移
                {
                    slave_response_data[2] = 0x08;                              // 上传数据字节数
                    slave_response_data[3] = 0x00;
                    slave_response_data[4] = datax[0];
                    slave_response_data[5] = datax[1];
                    slave_response_data[6] = datax[2]&0xF0;
                    slave_response_data[7] = 0x00;
                    slave_response_data[8] = datay[0];
                    slave_response_data[9] = datay[1];
                    slave_response_data[10] =datay[2]&0xF0;
                    slave_response_data_len = 11;
                }
				else if(ReadRegNum == 6) 
				{
				    slave_response_data[2] = 0x0C;                              // 上传数据字节数
                    slave_response_data[3] = 0x00;
                    slave_response_data[4] = datax[0];
                    slave_response_data[5] = datax[1];
                    slave_response_data[6] = datax[2]&0xF0;
                    slave_response_data[7] = 0x00;
                    slave_response_data[8] = datay[0];
                    slave_response_data[9] = datay[1];;
                    slave_response_data[10] =datay[2]&0xF0;
				    slave_response_data[11] = 0x00;
                    slave_response_data[12] = dataz[0];
                    slave_response_data[13] = dataz[1];
                    slave_response_data[14] = dataz[2]&0xF0;
                    slave_response_data_len = 15;
                }
                else
                    err = MDB_REG_ADDR_INVALID;									// 寄存器地址不正确
            }
            break;

			
		case ACC_ADDR_0x04:                                                     // 读位移
            if(ReadRegNum == 2)
            {
                slave_response_data[2] = 0x04;                                  // 上传数据字节数 输入寄存器为32位4个字节
                slave_response_data[3] = 0x00;
                slave_response_data[4] = datay[0];
                slave_response_data[5] = datay[1];;
                slave_response_data[6] =datay[2]&0xF0;
                slave_response_data_len = 7;
            }
			else if(ReadRegNum == 4)
			{
				slave_response_data[2] = 0x08;
				slave_response_data[7] = 0x00;
				slave_response_data[8] = datay[0];
				slave_response_data[9] = datay[1];;
				slave_response_data[10] =datay[2]&0xF0;
				slave_response_data[11] = 0x00;
				slave_response_data[12] = dataz[0];
				slave_response_data[13] = dataz[1];
				slave_response_data[14] = dataz[2]&0xF0;
                slave_response_data_len = 15;				
			}
			
			else
                err = MDB_REG_ADDR_INVALID;										// 寄存器地址不正确
            break;
			
		 		case ACC_ADDR_0x06:                                                     // 读位移
            if(ReadRegNum == 2)
            {
                slave_response_data[2] = 0x04;                                  // 上传数据字节数 输入寄存器为32位4个字节
                slave_response_data[3] = 0x00;
                slave_response_data[4] = datay[0];
                slave_response_data[5] = datay[1];;
                slave_response_data[6] =datay[2]&0xF0;
                slave_response_data_len = 7;
            }

			
			else
                err = MDB_REG_ADDR_INVALID;										// 寄存器地址不正确
            break;
            
        default:
            err = MDB_REG_ADDR_INVALID;                                         // 寄存器地址不正确
            break;
    }
    
    return err;
}


uint8_t MDB_F05_Write_Coil_Reg(void)											// 写线圈 标定功能
{
    uint8_t  err = MDB_NO_ERROR;
    uint16_t WriteCoilRegAddr = 0;
	
	WriteCoilRegAddr = (master_request_data[2] << 8) + master_request_data[3];
	
	switch(WriteCoilRegAddr)
    {

			
		case RESET_ADDR_0x51:                                                   // 恢复出厂设置寄存地址值
		  	err = MDB_F05_Write_Reset();
			break;
			
		default:
            err = MDB_REG_ADDR_INVALID;                                         // 寄存器地址不正确
            break;
	}
			
    return err;
}



uint8_t MDB_F05_Write_Reset() 													// 写恢复出厂设置线圈
{
  	uint8_t err = MDB_NO_ERROR;
	uint16_t WriteData = 0;
	
	WriteData = (master_request_data[4] << 8) + master_request_data[5];			// 写入的数据
	
	 switch(WriteData)
    {
		case 0x55AA:															// 将Flash参数备份到备份区	
			BackupCurParaToFlash();
			break;
			
		case 0xFF00:															// 恢复出厂设置
			FlashReadNHalfWord(STANDBY_BASE_ADDR, (uint16_t *)&FlashPara, 
						   sizeof(FlashPara_Typedef) >> 1);						// 将Flash备份参数区数据读出到当前参数区
			WriteCurParaToFlash();												// 将当前参数存入Flash当前参数区 设置完成后 单片机会自动重启 然后等待1.5秒左右启动完成
			MDBWriteFlag = MDBWriteReset;
			break;
			
		default:
            err = MDB_REG_VALUE_OVERRIDE;                                        // 寄存器值超出范围
            return err;
//			break;
	}
	
	slave_response_data[2] = RESET_ADDR_0x51 >> 8;								// 响应消息
	slave_response_data[3] = RESET_ADDR_0x51;
	slave_response_data[4] = master_request_data[4];
	slave_response_data[5] = master_request_data[5];
	slave_response_data_len = 6;
	
	return err;
}

uint8_t MDB_F10_Write_Hold_Reg(void)											// 写保持寄存器
{
  	uint8_t  err = MDB_NO_ERROR;
    uint16_t WriteHoldRegAddr = 0;                                              // 读保持寄存器地址值
    
    WriteHoldRegAddr = (master_request_data[2] << 8) + master_request_data[3];
    
    switch(WriteHoldRegAddr)
    {
        case SLAVE_ADDR_0x30:
        case BAUD_ADDR_0x31:  
        case PARITY_ADDR_0x32:                                                  // 写从机地址 波特率 奇偶校验单个或多个数据 使用同一个功能函数
            err = MDB_F10_Write_Addr_Baud_Parity();
            break;
            
  
		case FREQUENCY_ADDR_0x3F:												// 写补偿使能寄存器
        case OUT_PUT_MOD_ADDR_0x60:												// 写输出模式寄存器
            err = MDB_F10_Write_Single_Hold_Reg();								// 写单个保持寄存器
            break;
            
        default:
            err = MDB_REG_ADDR_INVALID;                                         // 寄存器地址不正确
            break;
    }
	
	WriteCurParaToFlash();														// 把当前参数写入Flash
	
    return err;
}

uint8_t MDB_F10_Write_Addr_Baud_Parity(void)									// 写从机地址 波特率 奇偶校验数据
{
	uint8_t  i = 0;
    uint8_t  err = MDB_NO_ERROR;
    uint16_t WriteRegNum = 0;													// 读寄存器数量
    uint16_t WriteRegStartAddr = 0;												// 读寄存器起始地址
    uint16_t WriteRegEndAddr = 0;												// 读寄存器结束地址
	uint16_t RegData[3] = {0};													// 寄存器数据缓存
	
	WriteRegNum = (master_request_data[4] << 8) + master_request_data[5];       // 写寄存器数量
	WriteRegStartAddr = (master_request_data[2] << 8) + master_request_data[3];	// 写入寄存器起始地址
	WriteRegEndAddr = WriteRegStartAddr + WriteRegNum - 1;						// 写入寄存器结束地址
	
	if(WriteRegEndAddr > PARITY_ADDR_0x32 || WriteRegNum > 3)                   // 地址超出范围
    {
        err = MDB_REG_ADDR_INVALID;												// 寄存器值超出范围
        return err;
    }
	
	RegData[0] = FlashPara.Customer.Addr;										// 必须先将当前参数赋值给数组 因为上位机写数据数据没确定
	RegData[1] = FlashPara.Customer.Baud;
	RegData[2] = FlashPara.Customer.Parity;
	
	for(i = 0; i < WriteRegNum; i++)											// 把要写的数据存放到WriteData数组 方便操作
	  	RegData[WriteRegStartAddr - 0x30 + i] = (master_request_data[7 + 2 * i] << 8) + 
		  										   master_request_data[8 + 2 * i];
	
	if(FlashPara.Customer.Baud != RegData[1] || FlashPara.Customer.Parity != RegData[2])	// 串口设置发生变更
		MDBWriteFlag = MDBWriteBaudOrParity;									// 写参数标记
	
	// 写入值范围不正确 则返回错误码
	if(RegData[0] == 0 || RegData[0] > 247 || 									// 地址值设定值范围为1~246
	   RegData[1] == 0 || RegData[1] > 7   || 									// 波特率设定值范围为1~7
	   RegData[2] == 0 || RegData[2] > 3)										// 奇偶校验设定值范围为1~3
    {
        err = MDB_REG_VALUE_OVERRIDE;                                           // 寄存器值超出范围
        return err;
    }
	
	FlashPara.Customer.Addr = RegData[0];										// 更新数据
	FlashPara.Customer.Baud = RegData[1];
	FlashPara.Customer.Parity = RegData[2];
	
	FlashPara.Common.Addr = (uint8_t)RegData[0];								// 通信参数区同步更新数据
	FlashPara.Common.Baud = (uint8_t)RegData[1];
	FlashPara.Common.Parity = (uint8_t)RegData[2];
	
	slave_response_data[2] = master_request_data[2];
	slave_response_data[3] = master_request_data[3];
	slave_response_data[4] = master_request_data[4];
	slave_response_data[5] = master_request_data[5];
	slave_response_data_len = 6;
	
    return err;
}

uint8_t MDB_F10_Write_Single_Hold_Reg(void)										// 写单个保持寄存器
{
  	uint8_t  err = MDB_NO_ERROR;
	uint16_t WriteRegAddr = 0;
	uint16_t WriteRegNum = 0;
	uint8_t  WriteByteNum = 0;
	uint16_t WriteData = 0;
    
	WriteRegAddr = (master_request_data[2] << 8) + master_request_data[3];		// 写寄存器地址
    WriteRegNum = (master_request_data[4] << 8) + master_request_data[5];		// 写寄存器数量
	WriteByteNum = master_request_data[6];										// 写入字节数
	WriteData = (master_request_data[7] << 8) + master_request_data[8];			// 写入寄存器的数据
	
	if(WriteRegNum != 1 || WriteByteNum !=2)									// 写单个寄存时 数据数量为1 数据字节数为2
    {
        err = MDB_REG_ADDR_INVALID;												// 寄存器值超出范围
        return err;
    }
    
    else
    {

		if(WriteRegAddr == FREQUENCY_ADDR_0x3F)									// 频率设定值只能是1~6
		{
			if(WriteData == 0 || WriteData > 8)
			{
				err = MDB_REG_VALUE_OVERRIDE;									// 寄存器值超出范围
				return err;
			}
			
	  		FlashPara.Customer.UploadFreq = WriteData;
			slave_response_data[2] = FREQUENCY_ADDR_0x3F >> 8;					// 寄存器地址高8位
			slave_response_data[3] = FREQUENCY_ADDR_0x3F;						// 寄存器地址低8位
			MDBWriteFlag = MDBWriteFrequency;									// 写参数标记
		}
		
		if(WriteRegAddr == OUT_PUT_MOD_ADDR_0x60)
		{
			if(WriteData != OUTPUTMOD_RTU && WriteData != OUTPUTMOD_ASCII)		// 输出模式设定值只能是RTU模式或ASCII模式
			{
				err = MDB_REG_VALUE_OVERRIDE;									// 寄存器值超出范围
				return err;
			}
			
	  		FlashPara.Customer.OutPutMod = WriteData;
			FlashPara.Common.OutPutMod = (uint8_t)WriteData;
			slave_response_data[2] = OUT_PUT_MOD_ADDR_0x60 >> 8;				// 寄存器地址高8位
			slave_response_data[3] = OUT_PUT_MOD_ADDR_0x60;						// 寄存器地址低8位
			MDBWriteFlag = MDBWriteOutPutMod;									// 写输出模式标记 等回应消息发送完成后 才将写入消息存入Out_Put_Mode变量中
		}
		
        slave_response_data[4] = 0x00;                							// 数据长度高8位
        slave_response_data[5] = 0x01;                     						// 数据长度低8位
        slave_response_data_len = 6;
    }
    
    return err;
}

uint8_t MDB_F25_Int_Write_Coil_Reg(void)										// 内部写线圈 标定功能
{
    uint8_t  err = MDB_NO_ERROR;
	uint16_t RegAddr = 0;
	
	RegAddr = (master_request_data[2] << 8) + master_request_data[3];			// 
	
	switch(RegAddr)
	{
		case UPGRADE_ADDR_0x40:                                          		// 系统在线升级寄存器地址值
		  	err = MDB_F25_Int_Write_Upgrade();
			break;
			

			
		default:
            err = MDB_REG_ADDR_INVALID;                                         // 寄存器地址不正确
            break;
    }
    
    return err;	
}


uint8_t MDB_F25_Int_Write_Upgrade(void)											// 写在线升级标志线圈
{
	uint8_t  err = MDB_NO_ERROR;
	uint16_t WriteData = 0;
	
	WriteData = (master_request_data[4] << 8) + master_request_data[5];			// 写入的数据
	
	if(WriteData == 0xFF00)														// 系统升级
	{
		FlashPara.Common.UpgradeFlag = 0xFF;
		FlashPara.Customer.UpgradeFlag = 0x0000;								// 升级标志值为零不变 仅对通用参数区升级标志参数进行更改 程序复位进入BootLoad升级完成后会对通用参数区升级标志复位为零
		MDBWriteFlag = MDBWriteUpgrade;											// 写参数标记
		
		slave_response_data[2] = UPGRADE_ADDR_0x40 >> 8;						// 响应消息
		slave_response_data[3] = UPGRADE_ADDR_0x40;
		slave_response_data[4] = master_request_data[4];
		slave_response_data[5] = master_request_data[5];
		slave_response_data_len = 6;
	}
	
	else
	{
		err = MDB_REG_VALUE_OVERRIDE;											// 寄存器值超出范围
		return err;
	}
	
	WriteCurParaToFlash();														// 把当前参数写入Flash
	
	return err;
}

uint8_t MDB_F27_Int_Write_Hold_Reg(void)										// 内部写保持寄存器
{
	uint8_t err = MDB_NO_ERROR;
	uint16_t StartAddr = 0;
	uint8_t  WriteFlag = 0;
	
	slave_response_data[0] = master_request_data[0];
	slave_response_data[1] = master_request_data[1];
	
	StartAddr = (master_request_data[2] << 8) + master_request_data[3];
	
	if(StartAddr >= PosStartAddr && StartAddr <= PosEndAddr)					// 写位移标定参数
		WriteFlag = WriteTsc;
	
	switch(WriteFlag)
	{
		case WriteTsc:
			err = MDB_F27_Int_Write_Pos_Reg(StartAddr);
			break;
			

			
		default:
			err = MDB_REG_ADDR_INVALID;                                        	// 寄存器地址不正确
			break;
	}
	
	return err;
}

uint8_t MDB_F27_Int_Write_Pos_Reg(uint16_t StartAddr)							// 内部写位移保持寄存器
{
	uint8_t  err = MDB_NO_ERROR;
	uint16_t i = 0;
	uint16_t WriteNum = 0;
	uint16_t WriteByte = 0;
	uint16_t EndAddr = 0;
	uint16_t *pTscPara;
	
	WriteNum = (master_request_data[4] << 8) + master_request_data[5];
	WriteByte = master_request_data[6];
	EndAddr = StartAddr + WriteNum - 1;
	
	if(EndAddr > PosEndAddr)													// 连续读结束地址是否超出范围
		return MDB_REG_ADDR_INVALID;
	
	if(WriteByte != 2 * WriteNum)
		return MDB_REG_VALUE_OVERRIDE;
	
	else
	{
		slave_response_data[2] = master_request_data[2];
		slave_response_data[3] = master_request_data[3];
		slave_response_data[4] = master_request_data[4];
		slave_response_data[5] = master_request_data[5];
		slave_response_data_len = 6;
		
		pTscPara = (uint16_t*)&FlashPara.Customer.TempH;									// 将指针指向位移参数结构体 方便数据操作
		for(i = 0; i < WriteNum; i++)
			pTscPara[StartAddr - PosStartAddr + i] = (master_request_data[7 + (i << 1)] << 8) + 
													  master_request_data[8 + (i << 1)];	
	}
	
	WriteCurParaToFlash();														// 将当前参数写入Flash
		
	return err;
}






void WriteCurParaToFlash(void)													// 把当前参数写入Flash 备份区不更新
{
	uint8_t times = 3;
	FlashPara_Typedef ReadFlashParaBuff;
	
	do       				                                               		// 写入的和读出的不相等则写3次
	{
		DIS_INT;
		FlashPageErase(FLASH_BASE_ADDR);										// Flash写数据之前 必须先擦除

		FlashWriteDoubleWord(FLASH_BASE_ADDR, (uint64_t *)&FlashPara, 
							sizeof(FlashPara_Typedef) >> 3);					// 将当前数据写入Flash当前参数区	
		
		FlashReadNHalfWord(FLASH_BASE_ADDR, (uint16_t *)&ReadFlashParaBuff, 
						   sizeof(FlashPara_Typedef) >> 1);						// 将Flash当前参数读出
		EN_INT;																	// 总中断失能到使能必须从擦除开始到写入成功
		
		if(!memcmp((uint8_t*)&ReadFlashParaBuff, (uint8_t*)&FlashPara, sizeof(FlashPara_Typedef)))	// 相等则退出
			break;
	}while(times--);															// 写入错误则重复写入不超过三次 确保写入正确
}

void BackupCurParaToFlash(void)													// 把当前参数保存到当前参数区和备份区 
{
	uint8_t times = 3;
	FlashPara_Typedef ReadFlashParaBuff;
	
	do       				                                               		// 写入的和读出的不相等则写3次
	{
		DIS_INT;
		FlashPageErase(STANDBY_BASE_ADDR);										// Flash写数据之前 必须先擦除
		
		FlashWriteDoubleWord(STANDBY_BASE_ADDR, (uint64_t *)&FlashPara, 
							sizeof(FlashPara_Typedef) >> 3);					// 将当前数据写入Flash当前参数区

		FlashReadNHalfWord(STANDBY_BASE_ADDR, (uint16_t *)&ReadFlashParaBuff, 
						   sizeof(FlashPara_Typedef) >> 1);						// 将Flash参数备份区数据读出
		EN_INT;																	// 总中断失能到使能必须从擦除开始到写入成功
		
		if(!memcmp((uint8_t*)&ReadFlashParaBuff, (uint8_t*)&FlashPara, sizeof(FlashPara_Typedef)))		// 相等则退出
			break;
	}while(times--);															// 写入错误则重复写入不超过三次 确保写入正确
}

void MDB_Snd_Msg(uint8_t *pMsg, uint8_t len)									// 发送消息
{
	uint8_t  i = 0;
	uint8_t  ASCIIData[DataPacketLen];
	uint8_t  LRCResult = 0;
	uint8_t  ucCRC[2] = {0};
	uint16_t uiCRC = 0;
	
  	if(Out_Put_Mode == OUTPUTMOD_RTU)											// RTU模式
	{
	  	uiCRC = MDB_Get_CRC16(pMsg, len);										// 获取CRC校验
		ucCRC[0] = (uint8_t)(uiCRC & 0x00FF);									// 低位在前
		ucCRC[1] = (uint8_t)((uiCRC >> 8) & 0x00FF);        					// 高位在后
		                   									
		
		
		RS485_TX_EN;															// 发送使能
		HAL_UART_Transmit(&hlpuart1, pMsg, len, 0xFFFF);							// 发送数据
		HAL_UART_Transmit(&hlpuart1, ucCRC, 2, 0xFFFF);							// 发送16位CRC码
		RS485_RX_EN;															// 接收使能
	}
	
	if(Out_Put_Mode == OUTPUTMOD_ASCII)											// ASCII模式
	{
		ASCIIData[0] = 0x3A;													// ASCII模式起始符
		
		for(i = 0; i < len; i++)
		{
			ASCIIData[(i << 1) + 1] = pMsg[i] >> 4;
			ASCIIData[(i << 1) + 2] = pMsg[i] & 0x0F;
		}
		
		LRCResult = MDB_Get_LRC(pMsg, len);										// 获取LRC校验
		ASCIIData[2 * len + 1] = LRCResult >> 4;
		ASCIIData[2 * len + 2] = LRCResult & 0x0F;
		
		for(i = 1; i <= 2 * len + 2; i++)										// 将数值转换成对应的ASCII码字符
		{
			if(ASCIIData[i] <= 9)
				ASCIIData[i] = ASCIIData[i] + 48;
			
			if(ASCIIData[i] >= 10 && ASCIIData[i] <= 15)
				ASCIIData[i] = ASCIIData[i] - 10 + 65;
		}
		
		ASCIIData[2 * len + 3] = 0x0D;											// ASCII模式结束符
		ASCIIData[2 * len + 4] = 0x0A;
		
		RS485_TX_EN;															// 发送使能
		HAL_UART_Transmit(&hlpuart1, ASCIIData, 2 * len + 5, 0xFFFF);				// 发送数据
		RS485_RX_EN;															// 接收使能
	}
	
	if(MDBWriteFlag != MDBWriteNone)
		WriteParaInit();														// 写参数时 部分参数与通信参数有关 需要等应答消息发送完成后 再去更改通信参数
	
//	if(Out_Put_Mode == OUTPUTMOD_RTU)											// ASCII模式 重启定时器 计时发送间隔时间
//	{
//		Tim2Flag = 0;
//		TIM2->CNT = 0;															// Tim2定时定数值清零 重新开始计时
//		HAL_TIM_Base_Start(&htim2);												// 启动定时器
//	}
}

void WriteParaInit(void)														// 写参数初始化
{
	switch(MDBWriteFlag)
	{
		case MDBWriteBaudOrParity:
			MX_TIM3_Init();														// TIM3定时时长与波特率有关 需要重新设定
			MX_LPUART1_UART_Init();												// 串口重新初始化 重设波特率
			break;
			
		case MDBWriteOutPutMod:													// 消息发送完成才对Out_Put_Mode的值进行更新 防止写输出模式时 本帧回应消息输出模式发生改变
			Out_Put_Mode = FlashPara.Customer.OutPutMod;						// Out_Put_Mode变量值更新
			break;
			
		case MDBWriteFrequency:
//			SamplingParaInit();
		    MX_TIM1_Init();

			SamplingFlag = SamplingUnfinish;
			break;
			
		case MDBWriteUpgrade:
			NVIC_SystemReset();													// 系统升级软件复位
			break;
			
		case MDBWriteReset:
			NVIC_SystemReset();													// 恢复出厂设置软件复位重启
			break;
			
		default:
			break;
	}
	
	MDBWriteFlag = MDBWriteNone;												// 清标记
}


