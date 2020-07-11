
#include "modbus.h"

uint8_t MDBWriteFlag = MDBWriteNone;											// �ñ�����¼д���ģʽ дʱ���Ϊ1(MDBWriteOutPutMod)
uint8_t slave_response_data_len = 0;                                        	// �ӻ���Ӧ��Ϣ֡����
uint8_t slave_response_data[DataPacketLen] = {0};                           	// �ӻ���Ӧ��Ϣ֡����

extern uint8_t RcvPacketLen;													// ���յ�������Ϣ֡����
extern uint16_t ParityErrorFlag;												// ������żУ������־
extern uint16_t Out_Put_Mode;
extern uint16_t SamplingFlag;													// ������ɱ�־ ��������ɺ� ��TIM4�ж��л���Ϊ���
extern UART_HandleTypeDef hlpuart1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern uint8_t Tim2Flag;
extern CustomerPara_Typedef Default_CustomerPara;
extern FlashPara_Typedef	FlashPara;											// Flash��������ṹ��
extern uint8_t master_request_data[DataPacketLen];
//extern PosSampling_Typedef  PosSampling;										// λ�Ʋ��������ṹ��
//extern TempSampling_Typedef TempSampling;										// �¶Ȳ��������ṹ��
//extern CalPara_Typedef		CalPara;											// �궨��������ṹ��
//extern TestPara_Typedef	  	Test;												// �ڲ����ݲ����ṹ��

// ��ȡ�ӻ���ַ
uint8_t MDB_Get_Slave_Addr(uint8_t *master_request_data)
{
	return *master_request_data;
}

// ���ܲ�������
uint8_t MDB_Function(void)
{
	
    uint8_t err = MDB_NO_ERROR;                                                 // ������    
    uint8_t MasterRequestFunction = 0;                                       	// ���������ܺ�

    MasterRequestFunction = master_request_data[1];
    
    slave_response_data[0] = master_request_data[0];							// �ӻ���Ӧ��ַΪ������ַ��㲥��ַ
    slave_response_data[1] = master_request_data[1];							// �ӻ���Ӧ���ܺ�
	
	if(master_request_data[0] == BROADCAST_ADDR_0x00)							// �㲥����ַ
		MDB_F03_Broadcast_Read_Slave_Addr();
	
	else
    switch(MasterRequestFunction)
    {
        case MDB_F03_READ_HOLD_REG:                                             // �����ּĴ�������
            err = MDB_F03_Read_Hold_Reg();
            break;
            
        case MDB_F04_READ_INPUT_REG:											// ������Ĵ�������
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
    
    if(err != MDB_NO_ERROR)                                                     // ��������֡�д���
    {
        slave_response_data[1] |= 0x80;
        slave_response_data[2]  = err;
        slave_response_data_len = 3;
    }

	MDB_Snd_Msg(slave_response_data, slave_response_data_len);					// ������Ϣ

	return err;
}

uint8_t FrameCheck(void)														// ����֡У����
{
	uint8_t err = MDB_NO_ERROR;
	
	if(ParityErrorFlag)															// ��żУ�����򴮿�֡У�������ֱ�ӷ��� ����Ӧ��
	{
		ParityErrorFlag = 0;
		err = MDB_MSG_PARITY_ERROR;
		return err;
	}
	
	if((FlashPara.Customer.OutPutMod == OUTPUTMOD_RTU && RcvPacketLen < 5) || 
	   (FlashPara.Customer.OutPutMod == OUTPUTMOD_ASCII && RcvPacketLen < 9))	// ֡���ȴ���
	{
		err = MDB_MSG_LEN_ERROR;
		return err;
	}
	
	if(FlashPara.Customer.OutPutMod == OUTPUTMOD_ASCII)							// ͨ��ΪASCIIģʽ
		DataTransform();														// ��ASCIIģʽ���յ�����ת����RTUģʽ ȥ����֡ͷ��֡β

	if(MDB_Check(master_request_data, RcvPacketLen))							// CRC/LRCУ��
	{
		err = MDB_MSG_FRAME_CHECK_ERROR;
		return err;
	}
		
	if(MDB_Get_Slave_Addr(master_request_data) != FlashPara.Customer.Addr && 	// �Ǳ�����ַ��㲥��ַ
	   MDB_Get_Slave_Addr(master_request_data) != BROADCAST_ADDR_0x00)
	{
		err = MDB_MSG_ADDR_ERROR;
		return err;
	}																

	return err;	
}

uint8_t MDB_F03_Broadcast_Read_Slave_Addr(void)									// �㲥����ַ
{
	uint8_t  err = MDB_NO_ERROR;
	uint16_t ReadAddr = 0;
	uint16_t ReadNum = 0;
	
	ReadAddr = (master_request_data[2] << 8) + master_request_data[3];			// ���Ĵ����ĵ�ֵַ
	ReadNum = (master_request_data[4] << 8) + master_request_data[5];			// ���Ĵ����ĸ���

	if(master_request_data[1] != MDB_F03_READ_HOLD_REG)							// �㲥����ַ����ʹ��03������
	{
		err = MDB_FNO_INVALID;													// ��֧�ֵĹ�����
		return err;
	}
	
	if(ReadAddr != SLAVE_ADDR_0x30 || ReadNum != 1)								// ��ֵַ���Ǵӻ���ַ�Ĵ�����ֵַ ���ȡ�Ĵ���������Ϊ1
	{
		err = MDB_REG_ADDR_INVALID;												// �Ĵ�����ַ������Χ
		return err;
	}
	
	slave_response_data[2] = 0x02;												// �����ֽ���
	slave_response_data[3] = FlashPara.Customer.Addr >> 8;						// ��ַ��8λ
	slave_response_data[4] = FlashPara.Customer.Addr;							// ��ַ��8λ
	slave_response_data_len = 5;
	
	return err;
}

uint8_t MDB_F03_Read_Hold_Reg(void)												// �����ּĴ���
{
    uint8_t  err = MDB_NO_ERROR;
    uint16_t ReadHoldRegAddr = 0;                                               // �����ּĴ�����ֵַ

    ReadHoldRegAddr = (master_request_data[2] << 8) + master_request_data[3];
    
    switch(ReadHoldRegAddr)
    {
        case SLAVE_ADDR_0x30:
        case BAUD_ADDR_0x31:  
        case PARITY_ADDR_0x32:                                                  // ���ӻ���ַ ������ ��żУ�鵥���������� ʹ��ͬһ�����ܺ���
            err = MDB_F03_Read_Addr_Baud_Parity();
            break;
            
      
		case FREQUENCY_ADDR_0x3F:												// ������Ƶ�ʼĴ���
        case OUT_PUT_MOD_ADDR_0x60:												// �����ģʽ�Ĵ���
            err = MDB_F03_Read_Single_Hold_Reg();
            break;
    
        default:
            err = MDB_REG_ADDR_INVALID;                                         // �Ĵ�����ַ����ȷ
            break;
    }
    
    return err;
}

uint8_t MDB_F03_Read_Addr_Baud_Parity(void)										// ���ӻ���ַ ������ ��żУ�鵥��������������
{
    uint8_t  i = 0;
    uint8_t  err = MDB_NO_ERROR;
    uint16_t ReadRegNum = 0;                                                    // ���Ĵ�������
    uint16_t ReadRegStartAddr = 0;                                              // ���Ĵ�����ʼ��ַ
    uint16_t ReadRegEndAddr = 0;                                                // ���Ĵ���������ַ
    uint16_t RegData[3] = {0};
    
    RegData[0] = FlashPara.Customer.Addr;                                       // ��Ҫ���Ĳ�������������� �������
    RegData[1] = FlashPara.Customer.Baud;
    RegData[2] = FlashPara.Customer.Parity;
    
    ReadRegNum = (master_request_data[4] << 8) + master_request_data[5];  
    ReadRegStartAddr = (master_request_data[2] << 8) + master_request_data[3];
    ReadRegEndAddr = ReadRegStartAddr + ReadRegNum - 1;

    if(ReadRegEndAddr > PARITY_ADDR_0x32 || ReadRegNum > 3)						// ��ַ������Χ
    {
        err = MDB_REG_ADDR_INVALID;												            // �Ĵ���ֵ������Χ
        return err;
    }
    
    else
    {
        slave_response_data[2] = 2 * ReadRegNum;                                // �ϴ������ֽ���
        for(i = 0; i < ReadRegNum; i++)
        {
            slave_response_data[3 + 2 * i] = RegData[ReadRegStartAddr - 0x30 + i] >> 8;     // ���ݸ�8λ
            slave_response_data[4 + 2 * i] = RegData[ReadRegStartAddr - 0x30 + i];          // ���ݵ�8λ
        }
        slave_response_data_len = 3 + 2 * ReadRegNum;                           // 
    }
     
    return err;
}

uint8_t MDB_F03_Read_Single_Hold_Reg(void)										// ������ʹ������
{
    uint8_t  err = MDB_NO_ERROR;
    uint16_t ReadRegNum = 0;
	uint16_t ReadRegAddr = 0;

	ReadRegAddr = (master_request_data[2] << 8) + master_request_data[3];		// ���Ĵ�����ַ
    ReadRegNum = (master_request_data[4] << 8) + master_request_data[5];		// ���Ĵ�������
    
	if(ReadRegNum != 1)                                                         // �������Ĵ�ʱ ��ֵΪ1
    {
        err = MDB_REG_ADDR_INVALID;												// �Ĵ���ֵ������Χ
        return err;
    }
    
    else
    {
        slave_response_data[2] = 0x02;                                          // �ϴ������ֽ��� �����Ĵ���Ϊ16λ2���ֽ�
		

		
		if(ReadRegAddr == FREQUENCY_ADDR_0x3F)
		{
			slave_response_data[3] = FlashPara.Customer.UploadFreq >> 8;			// ���ݸ�8λ
        	slave_response_data[4] = FlashPara.Customer.UploadFreq;				// ���ݵ�8λ
		}
		
		if(ReadRegAddr == OUT_PUT_MOD_ADDR_0x60)
		{
			slave_response_data[3] = FlashPara.Customer.OutPutMod >> 8;			// ���ݸ�8λ
        	slave_response_data[4] = FlashPara.Customer.OutPutMod;				// ���ݵ�8λ
		}
        
        slave_response_data_len = 5;
    }
    
    return err;
}

uint8_t MDB_F04_Read_Input_Reg(void)											// ������Ĵ��� λ�ƻ��¶� ReadRegNumΪ2ʱ������ Ϊ4ʱ����ͬʱ������
{
    uint8_t  err = MDB_NO_ERROR;
    uint16_t ReadRegNum = 0;
    uint16_t ReadRegAddr = 0;
    
    ReadRegNum = (master_request_data[4] << 8) + master_request_data[5];  
    ReadRegAddr = (master_request_data[2] << 8) + master_request_data[3];
    
    switch(ReadRegAddr)
    {  
        case ACC_ADDR_0x02:
            if(ReadRegNum == 2)                                                 // ���¶�
            {
                slave_response_data[2] = 0x04;                                  // �ϴ������ֽ��� ����Ĵ���Ϊ32λ4���ֽ�
                slave_response_data[3] = 0x00;
                slave_response_data[4] = datax[0];
                slave_response_data[5] = datax[1];
                slave_response_data[6] = datax[2]&0xF0;
                slave_response_data_len = 7;
            }
			
			else
            {
                if(ReadRegNum == 4)                                             // ���¶Ⱥ�λ��
                {
                    slave_response_data[2] = 0x08;                              // �ϴ������ֽ���
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
				    slave_response_data[2] = 0x0C;                              // �ϴ������ֽ���
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
                    err = MDB_REG_ADDR_INVALID;									// �Ĵ�����ַ����ȷ
            }
            break;

			
		case ACC_ADDR_0x04:                                                     // ��λ��
            if(ReadRegNum == 2)
            {
                slave_response_data[2] = 0x04;                                  // �ϴ������ֽ��� ����Ĵ���Ϊ32λ4���ֽ�
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
                err = MDB_REG_ADDR_INVALID;										// �Ĵ�����ַ����ȷ
            break;
			
		 		case ACC_ADDR_0x06:                                                     // ��λ��
            if(ReadRegNum == 2)
            {
                slave_response_data[2] = 0x04;                                  // �ϴ������ֽ��� ����Ĵ���Ϊ32λ4���ֽ�
                slave_response_data[3] = 0x00;
                slave_response_data[4] = datay[0];
                slave_response_data[5] = datay[1];;
                slave_response_data[6] =datay[2]&0xF0;
                slave_response_data_len = 7;
            }

			
			else
                err = MDB_REG_ADDR_INVALID;										// �Ĵ�����ַ����ȷ
            break;
            
        default:
            err = MDB_REG_ADDR_INVALID;                                         // �Ĵ�����ַ����ȷ
            break;
    }
    
    return err;
}


uint8_t MDB_F05_Write_Coil_Reg(void)											// д��Ȧ �궨����
{
    uint8_t  err = MDB_NO_ERROR;
    uint16_t WriteCoilRegAddr = 0;
	
	WriteCoilRegAddr = (master_request_data[2] << 8) + master_request_data[3];
	
	switch(WriteCoilRegAddr)
    {

			
		case RESET_ADDR_0x51:                                                   // �ָ��������üĴ��ֵַ
		  	err = MDB_F05_Write_Reset();
			break;
			
		default:
            err = MDB_REG_ADDR_INVALID;                                         // �Ĵ�����ַ����ȷ
            break;
	}
			
    return err;
}



uint8_t MDB_F05_Write_Reset() 													// д�ָ�����������Ȧ
{
  	uint8_t err = MDB_NO_ERROR;
	uint16_t WriteData = 0;
	
	WriteData = (master_request_data[4] << 8) + master_request_data[5];			// д�������
	
	 switch(WriteData)
    {
		case 0x55AA:															// ��Flash�������ݵ�������	
			BackupCurParaToFlash();
			break;
			
		case 0xFF00:															// �ָ���������
			FlashReadNHalfWord(STANDBY_BASE_ADDR, (uint16_t *)&FlashPara, 
						   sizeof(FlashPara_Typedef) >> 1);						// ��Flash���ݲ��������ݶ�������ǰ������
			WriteCurParaToFlash();												// ����ǰ��������Flash��ǰ������ ������ɺ� ��Ƭ�����Զ����� Ȼ��ȴ�1.5�������������
			MDBWriteFlag = MDBWriteReset;
			break;
			
		default:
            err = MDB_REG_VALUE_OVERRIDE;                                        // �Ĵ���ֵ������Χ
            return err;
//			break;
	}
	
	slave_response_data[2] = RESET_ADDR_0x51 >> 8;								// ��Ӧ��Ϣ
	slave_response_data[3] = RESET_ADDR_0x51;
	slave_response_data[4] = master_request_data[4];
	slave_response_data[5] = master_request_data[5];
	slave_response_data_len = 6;
	
	return err;
}

uint8_t MDB_F10_Write_Hold_Reg(void)											// д���ּĴ���
{
  	uint8_t  err = MDB_NO_ERROR;
    uint16_t WriteHoldRegAddr = 0;                                              // �����ּĴ�����ֵַ
    
    WriteHoldRegAddr = (master_request_data[2] << 8) + master_request_data[3];
    
    switch(WriteHoldRegAddr)
    {
        case SLAVE_ADDR_0x30:
        case BAUD_ADDR_0x31:  
        case PARITY_ADDR_0x32:                                                  // д�ӻ���ַ ������ ��żУ�鵥���������� ʹ��ͬһ�����ܺ���
            err = MDB_F10_Write_Addr_Baud_Parity();
            break;
            
  
		case FREQUENCY_ADDR_0x3F:												// д����ʹ�ܼĴ���
        case OUT_PUT_MOD_ADDR_0x60:												// д���ģʽ�Ĵ���
            err = MDB_F10_Write_Single_Hold_Reg();								// д�������ּĴ���
            break;
            
        default:
            err = MDB_REG_ADDR_INVALID;                                         // �Ĵ�����ַ����ȷ
            break;
    }
	
	WriteCurParaToFlash();														// �ѵ�ǰ����д��Flash
	
    return err;
}

uint8_t MDB_F10_Write_Addr_Baud_Parity(void)									// д�ӻ���ַ ������ ��żУ������
{
	uint8_t  i = 0;
    uint8_t  err = MDB_NO_ERROR;
    uint16_t WriteRegNum = 0;													// ���Ĵ�������
    uint16_t WriteRegStartAddr = 0;												// ���Ĵ�����ʼ��ַ
    uint16_t WriteRegEndAddr = 0;												// ���Ĵ���������ַ
	uint16_t RegData[3] = {0};													// �Ĵ������ݻ���
	
	WriteRegNum = (master_request_data[4] << 8) + master_request_data[5];       // д�Ĵ�������
	WriteRegStartAddr = (master_request_data[2] << 8) + master_request_data[3];	// д��Ĵ�����ʼ��ַ
	WriteRegEndAddr = WriteRegStartAddr + WriteRegNum - 1;						// д��Ĵ���������ַ
	
	if(WriteRegEndAddr > PARITY_ADDR_0x32 || WriteRegNum > 3)                   // ��ַ������Χ
    {
        err = MDB_REG_ADDR_INVALID;												// �Ĵ���ֵ������Χ
        return err;
    }
	
	RegData[0] = FlashPara.Customer.Addr;										// �����Ƚ���ǰ������ֵ������ ��Ϊ��λ��д��������ûȷ��
	RegData[1] = FlashPara.Customer.Baud;
	RegData[2] = FlashPara.Customer.Parity;
	
	for(i = 0; i < WriteRegNum; i++)											// ��Ҫд�����ݴ�ŵ�WriteData���� �������
	  	RegData[WriteRegStartAddr - 0x30 + i] = (master_request_data[7 + 2 * i] << 8) + 
		  										   master_request_data[8 + 2 * i];
	
	if(FlashPara.Customer.Baud != RegData[1] || FlashPara.Customer.Parity != RegData[2])	// �������÷������
		MDBWriteFlag = MDBWriteBaudOrParity;									// д�������
	
	// д��ֵ��Χ����ȷ �򷵻ش�����
	if(RegData[0] == 0 || RegData[0] > 247 || 									// ��ֵַ�趨ֵ��ΧΪ1~246
	   RegData[1] == 0 || RegData[1] > 7   || 									// �������趨ֵ��ΧΪ1~7
	   RegData[2] == 0 || RegData[2] > 3)										// ��żУ���趨ֵ��ΧΪ1~3
    {
        err = MDB_REG_VALUE_OVERRIDE;                                           // �Ĵ���ֵ������Χ
        return err;
    }
	
	FlashPara.Customer.Addr = RegData[0];										// ��������
	FlashPara.Customer.Baud = RegData[1];
	FlashPara.Customer.Parity = RegData[2];
	
	FlashPara.Common.Addr = (uint8_t)RegData[0];								// ͨ�Ų�����ͬ����������
	FlashPara.Common.Baud = (uint8_t)RegData[1];
	FlashPara.Common.Parity = (uint8_t)RegData[2];
	
	slave_response_data[2] = master_request_data[2];
	slave_response_data[3] = master_request_data[3];
	slave_response_data[4] = master_request_data[4];
	slave_response_data[5] = master_request_data[5];
	slave_response_data_len = 6;
	
    return err;
}

uint8_t MDB_F10_Write_Single_Hold_Reg(void)										// д�������ּĴ���
{
  	uint8_t  err = MDB_NO_ERROR;
	uint16_t WriteRegAddr = 0;
	uint16_t WriteRegNum = 0;
	uint8_t  WriteByteNum = 0;
	uint16_t WriteData = 0;
    
	WriteRegAddr = (master_request_data[2] << 8) + master_request_data[3];		// д�Ĵ�����ַ
    WriteRegNum = (master_request_data[4] << 8) + master_request_data[5];		// д�Ĵ�������
	WriteByteNum = master_request_data[6];										// д���ֽ���
	WriteData = (master_request_data[7] << 8) + master_request_data[8];			// д��Ĵ���������
	
	if(WriteRegNum != 1 || WriteByteNum !=2)									// д�����Ĵ�ʱ ��������Ϊ1 �����ֽ���Ϊ2
    {
        err = MDB_REG_ADDR_INVALID;												// �Ĵ���ֵ������Χ
        return err;
    }
    
    else
    {

		if(WriteRegAddr == FREQUENCY_ADDR_0x3F)									// Ƶ���趨ֵֻ����1~6
		{
			if(WriteData == 0 || WriteData > 8)
			{
				err = MDB_REG_VALUE_OVERRIDE;									// �Ĵ���ֵ������Χ
				return err;
			}
			
	  		FlashPara.Customer.UploadFreq = WriteData;
			slave_response_data[2] = FREQUENCY_ADDR_0x3F >> 8;					// �Ĵ�����ַ��8λ
			slave_response_data[3] = FREQUENCY_ADDR_0x3F;						// �Ĵ�����ַ��8λ
			MDBWriteFlag = MDBWriteFrequency;									// д�������
		}
		
		if(WriteRegAddr == OUT_PUT_MOD_ADDR_0x60)
		{
			if(WriteData != OUTPUTMOD_RTU && WriteData != OUTPUTMOD_ASCII)		// ���ģʽ�趨ֵֻ����RTUģʽ��ASCIIģʽ
			{
				err = MDB_REG_VALUE_OVERRIDE;									// �Ĵ���ֵ������Χ
				return err;
			}
			
	  		FlashPara.Customer.OutPutMod = WriteData;
			FlashPara.Common.OutPutMod = (uint8_t)WriteData;
			slave_response_data[2] = OUT_PUT_MOD_ADDR_0x60 >> 8;				// �Ĵ�����ַ��8λ
			slave_response_data[3] = OUT_PUT_MOD_ADDR_0x60;						// �Ĵ�����ַ��8λ
			MDBWriteFlag = MDBWriteOutPutMod;									// д���ģʽ��� �Ȼ�Ӧ��Ϣ������ɺ� �Ž�д����Ϣ����Out_Put_Mode������
		}
		
        slave_response_data[4] = 0x00;                							// ���ݳ��ȸ�8λ
        slave_response_data[5] = 0x01;                     						// ���ݳ��ȵ�8λ
        slave_response_data_len = 6;
    }
    
    return err;
}

uint8_t MDB_F25_Int_Write_Coil_Reg(void)										// �ڲ�д��Ȧ �궨����
{
    uint8_t  err = MDB_NO_ERROR;
	uint16_t RegAddr = 0;
	
	RegAddr = (master_request_data[2] << 8) + master_request_data[3];			// 
	
	switch(RegAddr)
	{
		case UPGRADE_ADDR_0x40:                                          		// ϵͳ���������Ĵ�����ֵַ
		  	err = MDB_F25_Int_Write_Upgrade();
			break;
			

			
		default:
            err = MDB_REG_ADDR_INVALID;                                         // �Ĵ�����ַ����ȷ
            break;
    }
    
    return err;	
}


uint8_t MDB_F25_Int_Write_Upgrade(void)											// д����������־��Ȧ
{
	uint8_t  err = MDB_NO_ERROR;
	uint16_t WriteData = 0;
	
	WriteData = (master_request_data[4] << 8) + master_request_data[5];			// д�������
	
	if(WriteData == 0xFF00)														// ϵͳ����
	{
		FlashPara.Common.UpgradeFlag = 0xFF;
		FlashPara.Customer.UpgradeFlag = 0x0000;								// ������־ֵΪ�㲻�� ����ͨ�ò�����������־�������и��� ����λ����BootLoad������ɺ���ͨ�ò�����������־��λΪ��
		MDBWriteFlag = MDBWriteUpgrade;											// д�������
		
		slave_response_data[2] = UPGRADE_ADDR_0x40 >> 8;						// ��Ӧ��Ϣ
		slave_response_data[3] = UPGRADE_ADDR_0x40;
		slave_response_data[4] = master_request_data[4];
		slave_response_data[5] = master_request_data[5];
		slave_response_data_len = 6;
	}
	
	else
	{
		err = MDB_REG_VALUE_OVERRIDE;											// �Ĵ���ֵ������Χ
		return err;
	}
	
	WriteCurParaToFlash();														// �ѵ�ǰ����д��Flash
	
	return err;
}

uint8_t MDB_F27_Int_Write_Hold_Reg(void)										// �ڲ�д���ּĴ���
{
	uint8_t err = MDB_NO_ERROR;
	uint16_t StartAddr = 0;
	uint8_t  WriteFlag = 0;
	
	slave_response_data[0] = master_request_data[0];
	slave_response_data[1] = master_request_data[1];
	
	StartAddr = (master_request_data[2] << 8) + master_request_data[3];
	
	if(StartAddr >= PosStartAddr && StartAddr <= PosEndAddr)					// дλ�Ʊ궨����
		WriteFlag = WriteTsc;
	
	switch(WriteFlag)
	{
		case WriteTsc:
			err = MDB_F27_Int_Write_Pos_Reg(StartAddr);
			break;
			

			
		default:
			err = MDB_REG_ADDR_INVALID;                                        	// �Ĵ�����ַ����ȷ
			break;
	}
	
	return err;
}

uint8_t MDB_F27_Int_Write_Pos_Reg(uint16_t StartAddr)							// �ڲ�дλ�Ʊ��ּĴ���
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
	
	if(EndAddr > PosEndAddr)													// ������������ַ�Ƿ񳬳���Χ
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
		
		pTscPara = (uint16_t*)&FlashPara.Customer.TempH;									// ��ָ��ָ��λ�Ʋ����ṹ�� �������ݲ���
		for(i = 0; i < WriteNum; i++)
			pTscPara[StartAddr - PosStartAddr + i] = (master_request_data[7 + (i << 1)] << 8) + 
													  master_request_data[8 + (i << 1)];	
	}
	
	WriteCurParaToFlash();														// ����ǰ����д��Flash
		
	return err;
}






void WriteCurParaToFlash(void)													// �ѵ�ǰ����д��Flash ������������
{
	uint8_t times = 3;
	FlashPara_Typedef ReadFlashParaBuff;
	
	do       				                                               		// д��ĺͶ����Ĳ������д3��
	{
		DIS_INT;
		FlashPageErase(FLASH_BASE_ADDR);										// Flashд����֮ǰ �����Ȳ���

		FlashWriteDoubleWord(FLASH_BASE_ADDR, (uint64_t *)&FlashPara, 
							sizeof(FlashPara_Typedef) >> 3);					// ����ǰ����д��Flash��ǰ������	
		
		FlashReadNHalfWord(FLASH_BASE_ADDR, (uint16_t *)&ReadFlashParaBuff, 
						   sizeof(FlashPara_Typedef) >> 1);						// ��Flash��ǰ��������
		EN_INT;																	// ���ж�ʧ�ܵ�ʹ�ܱ���Ӳ�����ʼ��д��ɹ�
		
		if(!memcmp((uint8_t*)&ReadFlashParaBuff, (uint8_t*)&FlashPara, sizeof(FlashPara_Typedef)))	// ������˳�
			break;
	}while(times--);															// д��������ظ�д�벻�������� ȷ��д����ȷ
}

void BackupCurParaToFlash(void)													// �ѵ�ǰ�������浽��ǰ�������ͱ����� 
{
	uint8_t times = 3;
	FlashPara_Typedef ReadFlashParaBuff;
	
	do       				                                               		// д��ĺͶ����Ĳ������д3��
	{
		DIS_INT;
		FlashPageErase(STANDBY_BASE_ADDR);										// Flashд����֮ǰ �����Ȳ���
		
		FlashWriteDoubleWord(STANDBY_BASE_ADDR, (uint64_t *)&FlashPara, 
							sizeof(FlashPara_Typedef) >> 3);					// ����ǰ����д��Flash��ǰ������

		FlashReadNHalfWord(STANDBY_BASE_ADDR, (uint16_t *)&ReadFlashParaBuff, 
						   sizeof(FlashPara_Typedef) >> 1);						// ��Flash�������������ݶ���
		EN_INT;																	// ���ж�ʧ�ܵ�ʹ�ܱ���Ӳ�����ʼ��д��ɹ�
		
		if(!memcmp((uint8_t*)&ReadFlashParaBuff, (uint8_t*)&FlashPara, sizeof(FlashPara_Typedef)))		// ������˳�
			break;
	}while(times--);															// д��������ظ�д�벻�������� ȷ��д����ȷ
}

void MDB_Snd_Msg(uint8_t *pMsg, uint8_t len)									// ������Ϣ
{
	uint8_t  i = 0;
	uint8_t  ASCIIData[DataPacketLen];
	uint8_t  LRCResult = 0;
	uint8_t  ucCRC[2] = {0};
	uint16_t uiCRC = 0;
	
  	if(Out_Put_Mode == OUTPUTMOD_RTU)											// RTUģʽ
	{
	  	uiCRC = MDB_Get_CRC16(pMsg, len);										// ��ȡCRCУ��
		ucCRC[0] = (uint8_t)(uiCRC & 0x00FF);									// ��λ��ǰ
		ucCRC[1] = (uint8_t)((uiCRC >> 8) & 0x00FF);        					// ��λ�ں�
		                   									
		
		
		RS485_TX_EN;															// ����ʹ��
		HAL_UART_Transmit(&hlpuart1, pMsg, len, 0xFFFF);							// ��������
		HAL_UART_Transmit(&hlpuart1, ucCRC, 2, 0xFFFF);							// ����16λCRC��
		RS485_RX_EN;															// ����ʹ��
	}
	
	if(Out_Put_Mode == OUTPUTMOD_ASCII)											// ASCIIģʽ
	{
		ASCIIData[0] = 0x3A;													// ASCIIģʽ��ʼ��
		
		for(i = 0; i < len; i++)
		{
			ASCIIData[(i << 1) + 1] = pMsg[i] >> 4;
			ASCIIData[(i << 1) + 2] = pMsg[i] & 0x0F;
		}
		
		LRCResult = MDB_Get_LRC(pMsg, len);										// ��ȡLRCУ��
		ASCIIData[2 * len + 1] = LRCResult >> 4;
		ASCIIData[2 * len + 2] = LRCResult & 0x0F;
		
		for(i = 1; i <= 2 * len + 2; i++)										// ����ֵת���ɶ�Ӧ��ASCII���ַ�
		{
			if(ASCIIData[i] <= 9)
				ASCIIData[i] = ASCIIData[i] + 48;
			
			if(ASCIIData[i] >= 10 && ASCIIData[i] <= 15)
				ASCIIData[i] = ASCIIData[i] - 10 + 65;
		}
		
		ASCIIData[2 * len + 3] = 0x0D;											// ASCIIģʽ������
		ASCIIData[2 * len + 4] = 0x0A;
		
		RS485_TX_EN;															// ����ʹ��
		HAL_UART_Transmit(&hlpuart1, ASCIIData, 2 * len + 5, 0xFFFF);				// ��������
		RS485_RX_EN;															// ����ʹ��
	}
	
	if(MDBWriteFlag != MDBWriteNone)
		WriteParaInit();														// д����ʱ ���ֲ�����ͨ�Ų����й� ��Ҫ��Ӧ����Ϣ������ɺ� ��ȥ����ͨ�Ų���
	
//	if(Out_Put_Mode == OUTPUTMOD_RTU)											// ASCIIģʽ ������ʱ�� ��ʱ���ͼ��ʱ��
//	{
//		Tim2Flag = 0;
//		TIM2->CNT = 0;															// Tim2��ʱ����ֵ���� ���¿�ʼ��ʱ
//		HAL_TIM_Base_Start(&htim2);												// ������ʱ��
//	}
}

void WriteParaInit(void)														// д������ʼ��
{
	switch(MDBWriteFlag)
	{
		case MDBWriteBaudOrParity:
			MX_TIM3_Init();														// TIM3��ʱʱ���벨�����й� ��Ҫ�����趨
			MX_LPUART1_UART_Init();												// �������³�ʼ�� ���貨����
			break;
			
		case MDBWriteOutPutMod:													// ��Ϣ������ɲŶ�Out_Put_Mode��ֵ���и��� ��ֹд���ģʽʱ ��֡��Ӧ��Ϣ���ģʽ�����ı�
			Out_Put_Mode = FlashPara.Customer.OutPutMod;						// Out_Put_Mode����ֵ����
			break;
			
		case MDBWriteFrequency:
//			SamplingParaInit();
		    MX_TIM1_Init();

			SamplingFlag = SamplingUnfinish;
			break;
			
		case MDBWriteUpgrade:
			NVIC_SystemReset();													// ϵͳ���������λ
			break;
			
		case MDBWriteReset:
			NVIC_SystemReset();													// �ָ��������������λ����
			break;
			
		default:
			break;
	}
	
	MDBWriteFlag = MDBWriteNone;												// ����
}


