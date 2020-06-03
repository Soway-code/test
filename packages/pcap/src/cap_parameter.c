#include "BSP.h"
#include "eeprom.h"

uint8_t Onlineflg;

//const
uint8_t SensorSoftVersion[8] = {0x07, 'S', 'V', '3', '.', '1', '.', '4'};      //����汾
//const
uint8_t Cap_User_Default_Param[PRO_DEFAULT_LEN] =
{
	0x01,								        //������ʼ����־λ
        
 	0x41,								        //�豸��ַ ori 0x41
	0x03,								        //������(9600)
	0x03,								        //��żУ��(��У��)
	0x01,								        //�˲�ϵ��
	0x01,								        //�Զ���������
	0x3F, 0x80, 0x00, 0x00,			//����ϵ��K
	0x3F, 0x80, 0x00, 0x00,			//����ϵ��B
	0x01,								        //����ʹ��
	0x00,								        //�Ƿ񶳽��豸      

	0x00, 0x78,						    //����������(0.1mm)30DE4
	0x00, 0x03, 0x0D, 0xE4,		//������ֵ 0x00, 0x0C, 0x48, 0x5D,
	0x00, 0x05, 0x61, 0x32,		//�����̵���ֵ  //ori 0x00, 0x0D, 0xCF, 0x63,
	0x00, 0x00, 0x00, 0x00,		//���DAֵ
	0x45, 0x3B, 0x80, 0x00,		//����DAֵ
	
	0x00, 0x00, 0x00, 0x00,		//��������
	0x00, 0x00, 0x00, 0x00,		//����������ֵ
	0x00, 0x00, 0x00, 0x00,		//���������̵���ֵ
	0x3F, 0x80, 0x00, 0x00,		//�¶�ϵ��K
	0x00, 0x00, 0x00, 0x00,		//�¶�ϵ��B
	0x3F, 0x80, 0x00, 0x00,		//�¶�ϵ��K
	0x00, 0x00, 0x00, 0x00,		//�¶�ϵ��B
  0x00, 0x00, 0x00, 0x00,   //ҺλDA���ֵ
	0x03,                     //Һλ�Ƚ�ʱ����s  2 3 4 5
  0x04,                     //ȼ��Ĭ�ϲ���
  0x01,                      //������״Ĭ�ϳ�����
  0x05,0xDC,                 //���䳤1500
  0x02,0x58,                 //�����600
  0x01,0xF4,                 //�����500
  0x00,0x03,                 //�����3
  0x00,0x5A,                 //���ͳ���ʱ��s
  0x01,0x09,                 //��������ֵ0.1L
  0x00,0x5A,                 //©�ͳ���ʱ��s
  0x01,0x09,                  //©������ֵ0.1L
};

uint8_t Cur_Param[PRO_DEFAULT_LEN] = {0};                   //ϵͳ��ǰ����
uint8_t Calib_Param[CALIB_DEFAULT_LEN] = {0};                //�궨����
uint8_t CALIB_Data[2] = {0};

uint8_t CALIB_Table[CALIN_HEIGH_TABLE_NUM];                  //�����궨����

ProductParaTypeDef ProductPara;                         //��Ʒ����
RunVarTypeDef RunVar;                                  //���б���
UartCommTypeDef UartComm;                                //���ڲ���
UserParamTypeDef UserParam;                             //�û�����


//��ʼ��IIC�ӿ�
void AT24CXX_Init(void)
{
//    IIC_Init();
}

//��AT24CXXָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ
//����ֵ  :����������
uint8_t I2C1_ReadByte(uint16_t DriverAddr, uint16_t ReadAddr)
{
//    CPU_SR_ALLOC();
//    OS_CRITICAL_ENTER();
//
//    uint8_tuint8_t temp = 0;
//    IIC_Start();
//
//    if (EE_TYPE > AT24C16)
//    {
//        IIC_Send_Byte(DriverAddr);	   //����д����
//        IIC_Wait_Ack();
//        IIC_Send_Byte(ReadAddr >> 8);  //���͸ߵ�ַ
//        IIC_Wait_Ack();
//    }
//    else
//    { 
//        IIC_Send_Byte(DriverAddr + ((ReadAddr / 256) << 1));        //����������ַ0XA0,д����
//    }
//	
//    IIC_Wait_Ack();
//
//    IIC_Send_Byte(ReadAddr % 256);     //���͵͵�ַ
//
//    IIC_Wait_Ack();
//
//    IIC_Start();
//
//    IIC_Send_Byte(0XA1);           //�������ģʽ
//
//    IIC_Wait_Ack();
//
//    temp = IIC_Read_Byte(0);
//
//    IIC_Stop();//����һ��ֹͣ����
//
//    OS_CRITICAL_EXIT();

    return 0;//temp;
}

//��AT24CXXָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ
//DataToWrite:Ҫд�������
void I2C1_WriteByte(uint16_t DriverAddr, uint16_t WriteAddr, uint8_t DataToWrite)
{
//    CPU_SR_ALLOC();
//    
//    if(ProductPara.bFlashWEn == FLASH_WR_DISABLE)
//    {
//        return;
//    }
//    
//    OS_CRITICAL_ENTER();
//
//    IIC_Start();
//
//    if (EE_TYPE > AT24C16)
//    {
//        IIC_Send_Byte(DriverAddr);	    //����д����
//        IIC_Wait_Ack();
//        IIC_Send_Byte(WriteAddr >> 8);  //���͸ߵ�ַ
//    }
//    else
//    {
//        IIC_Send_Byte(DriverAddr + ((WriteAddr / 256) << 1));        //����������ַ0XA0,д����
//    }
//
//    IIC_Wait_Ack();
//
//    IIC_Send_Byte(WriteAddr % 256);     //���͵͵�ַ
//    IIC_Wait_Ack();
//    IIC_Send_Byte(DataToWrite);     //�����ֽ�
//    IIC_Wait_Ack();
//    IIC_Stop();//����һ��ֹͣ����
//    Delay_Ms(2);
//
//    OS_CRITICAL_EXIT();
}


//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
void I2C1_ReadNBytes(uint16_t DriverAddr, uint16_t ReadAddr, uint16_t NumToRead, uint8_t *pBuffer)
{
//    while (NumToRead)
//    {
//        *pBuffer++ = I2C1_ReadByte(DriverAddr, ReadAddr++);
//        NumToRead--;
//    }
}

//��AT24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
void I2C1_WriteNBytes(uint16_t DriverAddr, uint16_t WriteAddr, uint16_t NumToWrite, const uint8_t *pBuffer)
{
//    if(ProductPara.bFlashWEn == FLASH_WR_DISABLE)
//    {
//        return;
//    }
//    
//    while (NumToWrite--)
//    {
//        I2C1_WriteByte(DriverAddr, WriteAddr, *pBuffer);
//        WriteAddr++;
//        pBuffer++;
//    }
}

//��AT24CXXָ����ַд������ָ����������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ
//DataToWrite:Ҫд�������
//pBuffer   :���������׵�ַ
void I2C1_WNBytesMul3T(uint16_t DriverAddr, uint16_t WriteAddr, uint16_t NumToWrite, const uint8_t *pBuffer)
{
//    if(ProductPara.bFlashWEn == FLASH_WR_DISABLE)
//    {
//        return;
//    }
//    
//    while (NumToWrite--)
//    {
//        I2C1_WriteByte(DriverAddr, WriteAddr, *pBuffer);
//        I2C1_WriteByte(DriverAddr, WriteAddr + 2 * 0x80, *pBuffer);
//        I2C1_WriteByte(DriverAddr, WriteAddr + 4 * 0x80, *pBuffer);
//        WriteAddr++;
//        pBuffer++;
//    }
}

//��AT24CXX��ʼ��
void EEProm_Init(void)
{
    AT24CXX_Init();
}


void Switch_Fiter(uint8_t value)
{
    switch(value)
    {	
    case 1:
        UserParam.FilterBufMax = 1;    
        UserParam.FilterCycle = 1;      //ʵʱ�˲�1s
        break;

    case 2:	
        UserParam.FilterBufMax = 18;    
        UserParam.FilterCycle = 180;    //ƽ���˲�180s
        break;

    case 3:	
        UserParam.FilterBufMax = 96;    //ƽ���˲�960s
        UserParam.FilterCycle = 960;
        break;

    default:
        UserParam.FilterBufMax = 18;    
        UserParam.FilterCycle = 180;    //ʵʱ�˲�1s
        break;
    }
}

//����û�����
void Check_User_Backup(void)
{
    uint32_t i = 0, Eerrno = 0;
    uint8_t bak1 = 0, bak2 = 0;
    
    for (i = 0; i < PRO_DEFAULT_LEN; i++)
    {
        Eerrno = 0;
        //��������1�ͱ���2����
//        bak1 = I2C1_ReadByte(EEPROM_ADDRESS, i + USER_DEFAULT_PARA_BAK1);
//        bak2 = I2C1_ReadByte(EEPROM_ADDRESS, i + USER_DEFAULT_PARA_BAK2);
		Eeprom_ReadNBytes(i + USER_DEFAULT_PARA_BAK1, &bak1, 1);
		Eeprom_ReadNBytes(i + USER_DEFAULT_PARA_BAK2, &bak2, 1);
		
//	  rt_kprintf("bak1 %x %x\n" ,bak1,Cur_Param[i]);
//		rt_kprintf("bak2 %x %x\n" ,bak2,Cur_Param[i]);
        //��ǰ�����뱸��1�Ƚ�
        if (Cur_Param[i] != bak1)
            Eerrno |= 0x01;
        //��ǰ�����뱸��2�Ƚ�
        if (Cur_Param[i] != bak2)
            Eerrno |= 0x02;
        //����1�뱸��2�Ƚ�
        if (bak1 != bak2)
            Eerrno |= 0x04;
        //�޴���
        if(0x00 == Eerrno)
            continue;
        //����������Ͳ�����
//      rt_kprintf("Check_User_Backup_Eerrno %x\n",Eerrno);
        switch (Eerrno)
        {
        case 0x03:
          Cur_Param[i] = bak1;
//          I2C1_WriteByte(EEPROM_ADDRESS, RUN_ADDR_BASE + i, Cur_Param[i]);
		  Eeprom_WriteNBytes(RUN_ADDR_BASE + i,&Cur_Param[i],1);
//			rt_kprintf("update %x\n",Cur_Param[i]);
          break;
        case 0x05:
//          I2C1_WriteByte(EEPROM_ADDRESS, USER_DEFAULT_PARA_BAK1 + i, Cur_Param[i]);
		  Eeprom_WriteNBytes(USER_DEFAULT_PARA_BAK1 + i,&Cur_Param[i],1);
          break;
        case 0x06:
//          I2C1_WriteByte(EEPROM_ADDRESS, USER_DEFAULT_PARA_BAK2 + i, Cur_Param[i]);
		 Eeprom_WriteNBytes(USER_DEFAULT_PARA_BAK2 + i,&Cur_Param[i],1);
          break;
        case 0x07:
          Cur_Param[i] = Cap_User_Default_Param[i];
//          I2C1_WNBytesMul3T(EEPROM_ADDRESS, RUN_ADDR_BASE + i, 1, &Cur_Param[i]);
		 Eeprom_WriteNBytes(RUN_ADDR_BASE + i,&Cur_Param[i],1);
		
          break;
        }
    }    
}

//���궨����
void Check_Calib_Backup(void)
{
    uint32_t i = 0, Eerrno = 0;
    uint8_t bak1 = 0, bak2 = 0;

    for(i = 0; i < CALIB_DEFAULT_LEN; i++)
    {
      Eerrno = 0;
      //��������1�ͱ���2����
//     bak1 = I2C1_ReadByte(EEPROM_ADDRESS, i + CALIB_DEFAULT_PARA_BAK1);
//      bak2 = I2C1_ReadByte(EEPROM_ADDRESS, i + CALIB_DEFAULT_PARA_BAK2);
	  Eeprom_ReadNBytes(i + CALIB_DEFAULT_PARA_BAK1, &bak1, 1);
	  Eeprom_ReadNBytes(i + CALIB_DEFAULT_PARA_BAK2, &bak2, 1);
      //��ǰ�����뱸��1�Ƚ�
      if (Calib_Param[i] != bak1)
        Eerrno |= 0x01;
      //��ǰ�����뱸��2�Ƚ�
      if (Calib_Param[i] != bak2)
        Eerrno |= 0x02;
      //����1�뱸��2�Ƚ�
      if (bak1 != bak2)
        Eerrno |= 0x04;
      //�޴���
      if(0x00 == Eerrno)
        continue;
      //����������Ͳ�����  
	  rt_kprintf("Check_Calib_Backup_Eerrno %xn",Eerrno);
      switch (Eerrno)
      {
	  
      case 0x03:
        Calib_Param[i] = bak1;
//        I2C1_WriteByte(EEPROM_ADDRESS, CALIB_FLOATER_SCALE_1 + i, Calib_Param[i]);	  
		Eeprom_WriteNBytes(CALIB_FLOATER_SCALE_1 + i,&Calib_Param[i],1);
        break;
      case 0x05:
//        I2C1_WriteByte(EEPROM_ADDRESS, CALIB_DEFAULT_PARA_BAK1 + i, Calib_Param[i]);
	    Eeprom_WriteNBytes(CALIB_DEFAULT_PARA_BAK1 + i,&Calib_Param[i],1);
        break;
      case 0x06:
//        I2C1_WriteByte(EEPROM_ADDRESS, CALIB_DEFAULT_PARA_BAK2 + i, Calib_Param[i]);
	   Eeprom_WriteNBytes(CALIB_DEFAULT_PARA_BAK2 + i,&Calib_Param[i],1);
        break;
      case 0x07:
        Calib_Param[i] = 0xFF;
//        I2C1_WNBytesMul3T(EEPROM_ADDRESS, CALIB_FLOATER_SCALE_1 + i , 1, &Calib_Param[i]);
	   Eeprom_WriteNBytes(CALIB_FLOATER_SCALE_1 + i,&Calib_Param[i],1);

		break;
      }
    }	
}

//��ȡ�豸����
void Rd_Dev_Param(void)
{
    uint8_t di;
    uint8_t ptmp[8];
	uint8_t ParaInitFlag;
	uint8_t write_zero[1]={0x00};
    //u16 ci,cj;
    //u32 oiltmp;

    ProductPara.bFlashWEn = FLASH_WR_ENABLE;
    
	Eeprom_ReadNBytes(RUN_ADDR_BASE, &ParaInitFlag, 1);
	//ParaInitFlag =0x02;
    if(Cap_User_Default_Param[0] != ParaInitFlag)	//��EEPROM�����ݣ�д�����ֵ
    {
      //д������ȱʡֵ               //CALIB_DEFAULT_LEN
     // I2C1_WNBytesMul3T(EEPROM_ADDRESS, RUN_ADDR_BASE, USER_DEFAULT_LEN, Cap_User_Default_Param);//д�����������궨������д
	  Eeprom_WriteNBytes(RUN_ADDR_BASE, Cap_User_Default_Param, PRO_DEFAULT_LEN);// ori USER_DEFAULT_LEN PRO_DEFAULT_LEN
	  
	  //rt_kprintf("Cap_User_Default_Param\n");

	}
    //�������������־��Ч�Ͳ���ΪĬ��ֵ
    
	Eeprom_ReadNBytes(ONLINEUPGRADE, &ParaInitFlag, 1);
    if(0x0C != ParaInitFlag)
    {
        //I2C1_WriteByte(EEPROM_ADDRESS, ONLINEUPGRADE, 0x00);
		Eeprom_WriteNBytes(ONLINEUPGRADE, write_zero, 1);
    }
    //д������汾
    //I2C1_WriteNBytes(EEPROM_ADDRESS, SENSOR_SV, 8, SensorSoftVersion);
	Eeprom_WriteNBytes(SENSOR_SV,SensorSoftVersion,8);
    //��ȡ�û�����
//    I2C1_ReadNBytes(EEPROM_ADDRESS, RUN_ADDR_BASE, PRO_DEFAULT_LEN, Cur_Param);
	Eeprom_ReadNBytes(RUN_ADDR_BASE, Cur_Param, PRO_DEFAULT_LEN);
//rt_kprintf("cap_Cur_Param %x %x %x\n",Cur_Param[0],Cur_Param[1],Cur_Param[2]);
    //��ȡ�͸��궨����
//    I2C1_ReadNBytes(EEPROM_ADDRESS, CALIB_FLOATER_SCALE_1, CALIB_DEFAULT_LEN, Calib_Param);
	Eeprom_ReadNBytes(CALIB_FLOATER_SCALE_1, Calib_Param, CALIB_DEFAULT_LEN);
	
//    Check_User_Backup();
    Check_Calib_Backup();
    
    //��ȡ�궨�¶� ����
//    I2C1_ReadNBytes(EEPROM_ADDRESS, CALIBTEMPER, 2, ptmp);
	Eeprom_ReadNBytes(CALIBTEMPER, ptmp, 2);
    ProductPara.CalibTemp = (ptmp[0] << 8)|ptmp[1];
    //��ȡ��������
//    I2C1_ReadNBytes(EEPROM_ADDRESS, CALIBOILRAG, 4, ptmp);
	Eeprom_ReadNBytes(CALIBOILRAG, ptmp, 4);
    ProductPara.OilRange = 0;
    for(di = 0; di < 4; di++)
    {
      ProductPara.OilRange = (ProductPara.OilRange << 8) + ptmp[di];
    }
        
    //�ӻ���ַ��Χ
   /* if((0x41 > Cur_Param[SLAVE_ADDR])||(0x42 < Cur_Param[SLAVE_ADDR]))
    {
      Cur_Param[SLAVE_ADDR] = 0x41;
    }*/
    //�����ʷ�Χ
    if((0x01 > Cur_Param[BAUDRATE])||(0x07 < Cur_Param[BAUDRATE]))
    {
      Cur_Param[BAUDRATE] = 0x03;
    }
    //��żУ�鷶Χ
    if((0x01 > Cur_Param[PARITY])||(0x03 < Cur_Param[PARITY]))
    {
      Cur_Param[PARITY] = 0x03;
    }
		
    //�ӻ���ַ
    UartComm.SlaveAddr = Cur_Param[SLAVE_ADDR];
	//	rt_kprintf("UartComm.SlaveAddr %x %x\n",UartComm.SlaveAddr,Cur_Param[1]);
    //������
    UartComm.BaudRate = Cur_Param[BAUDRATE];
    //��żУ��
    UartComm.Parity = Cur_Param[PARITY];
    //�˲��ȼ�
    UserParam.FilterLevel = Cur_Param[FILTER_LEVEL];
    //�˲��л�
    Switch_Fiter(UserParam.FilterLevel);
    //�Զ��ϴ�����
    ProductPara.AutoUpLoadTime = Cur_Param[AUTO_UPLOAD_TIME];
    //����ϵ��K
    UserParam.UserFlowK = HexToFloat(&Cur_Param[USER_FLOW_K]);
    //����ϵ��B
    UserParam.UserFlowB = HexToFloat(&Cur_Param[USER_FLOW_B]);
	
    //����ʹ��
    ProductPara.CompenEn = Cur_Param[COMPENSATE];
    //�Ƿ񶳽�
    UartComm.Echo = Cur_Param[ECHOEN];

    //�������
    ProductPara.CapMin = ArrToHex(&Cur_Param[CAPMIN]);
    //��������
    ProductPara.CapMax = ArrToHex(&Cur_Param[CAPMAX]);
    //��������ֵ
    ProductPara.AddLiqCnt = (Cur_Param[ADDLIQLMT] << 8)|Cur_Param[ADDLIQLMT +1];
    //���μ�����ֵ
    ProductPara.RestLiqCnt = ProductPara.AddLiqCnt / 2;
    //����ʱ����ֵ
    ProductPara.AddLiqTime = (Cur_Param[ADDLIQTIME] << 8)|Cur_Param[ADDLIQTIME +1];
    //©������ֵ
    ProductPara.SubLiqCnt = (Cur_Param[SUBLIQLMT] << 8)|Cur_Param[SUBLIQLMT +1];
    //©��ʱ����ֵ
    ProductPara.SubLiqTime = (Cur_Param[SUBLIQTIME] << 8)|Cur_Param[SUBLIQTIME +1];
    //DA���
    ProductPara.DAMin = HexToFloat(&Cur_Param[DAMIN]);
    //DA����
    ProductPara.DAMax = HexToFloat(&Cur_Param[DAMAX]);
    //������������
    ProductPara.Def_Range = HexToFloat(&Cur_Param[DEF_RANGE]);
    //�����������
    ProductPara.Def_CapMin = HexToFloat(&Cur_Param[DEF_CAPMIN]);
    //������������
    ProductPara.Def_CapMax = HexToFloat(&Cur_Param[DEF_CAPMAX]);

    //�¶�1ϵ��K
    ProductPara.Temp_K = HexToFloat(&Cur_Param[TEMP_K]);
    //�¶�1ϵ��B
    ProductPara.Temp_B = HexToFloat(&Cur_Param[TEMP_B]);
    //�¶�2ϵ��K
    ProductPara.Temp2_K = HexToFloat(&Cur_Param[TEMP2_K]);
    //�¶�2ϵ��B
    ProductPara.Temp2_B = HexToFloat(&Cur_Param[TEMP2_B]);
    //Һλ�Ƚ�ʱ����    
    if((0x02 > Cur_Param[LIQUIDCMPTC]) || (0x05 < Cur_Param[LIQUIDCMPTC]))
    {
      Cur_Param[LIQUIDCMPTC] = 0x03;
//      I2C1_WNBytesMul3T(EEPROM_ADDRESS, LIQUIDCMPTC, 1, &Cur_Param[LIQUIDCMPTC]);
	  Eeprom_WriteNBytes(LIQUIDCMPTC, &Cur_Param[LIQUIDCMPTC], 1);
	}
    ProductPara.LiquidCmpTc = Cur_Param[LIQUIDCMPTC];
    
    
    ProductPara.FuelType = Cur_Param[FUELTYPE];                                 //ȼ������
    ProductPara.BoxShape = Cur_Param[BOXSHAPE];                                 //������״
    ProductPara.BoxPara[0] = (Cur_Param[BOXPARA0] << 8)|Cur_Param[BOXPARA0 +1]; //��
    ProductPara.BoxPara[1] = (Cur_Param[BOXPARA1] << 8)|Cur_Param[BOXPARA1 +1]; //��
    ProductPara.BoxPara[2] = (Cur_Param[BOXPARA2] << 8)|Cur_Param[BOXPARA2 +1]; //��
    ProductPara.BoxPara[3] = (Cur_Param[BOXPARA3] << 8)|Cur_Param[BOXPARA3 +1]; //��
    /*
    ProductPara.BottomArea = (ProductPara.BoxPara[0] - 2*ProductPara.BoxPara[3])
                                * (ProductPara.BoxPara[1] - 2*ProductPara.BoxPara[3]) / 1000000.0f;//ƽ����
    ProductPara.AddMapHeight = ProductPara.AddLiqCnt * 100.0f / ProductPara.AddLiqTime / ProductPara.BottomArea;//���õ���ֵ��ӳ���ĸ߶�*1000mm
    ProductPara.SubMapHeight = ProductPara.SubLiqCnt * 100.0f / ProductPara.SubLiqTime / ProductPara.BottomArea;
    */
    //Һλ���DA  ����    
    ProductPara.LiquitHeightforDAMax = HexToFloat(&Cur_Param[HEIGHTDAMAX]);
    ProductPara.SensorLen = (Cur_Param[RANGE] << 8) + Cur_Param[RANGE + 1];     //����������
    ProductPara.Range = ProductPara.SensorLen - 330;                            //����������=����-ä��������ä���͹̶�33mm��
		rt_kprintf("rd_eep senslen&range %x %x\n",ProductPara.SensorLen,ProductPara.Range);

    //�͸��궨����
    for(di=0; di < 8; di++)
    {
      //I2C1_ReadNBytes(EEPROM_ADDRESS, CALIB_FLOATER_DA_1 + 2 * di, 2, CALIB_Data);
	  Eeprom_ReadNBytes(CALIB_FLOATER_DA_1 + 2 * di, CALIB_Data, 2);
      ProductPara.Floater.DACode[di] = CALIB_Data[1] * 256 + CALIB_Data[0];
//      I2C1_ReadNBytes(EEPROM_ADDRESS, CALIB_FLOATER_SCALE_1 + 2 * di, 2, CALIB_Data);
	  Eeprom_ReadNBytes(CALIB_FLOATER_SCALE_1 + 2 * di, CALIB_Data, 2);
      ProductPara.Floater.Scale[di] = CALIB_Data[1] * 256 + CALIB_Data[0];
    }
    //�����궨����    
//    I2C1_ReadNBytes(EEPROM_ADDRESS, CALIB_HEIGH_RELATIONSHIP, CALIN_HEIGH_TABLE_NUM, CALIB_Table);
	Eeprom_ReadNBytes(CALIB_HEIGH_RELATIONSHIP, CALIB_Table, CALIN_HEIGH_TABLE_NUM);

	//��������
    ProductPara.CapRange = ProductPara.CapMax - ProductPara.CapMin;
    //������ʼ����־
    RunVar.OilInit = 1;
    //Flashдʹ�ܽ�ֹ
    //UserParam.FilterM = Cur_Param[FILTER_M];
    //UserParam.FilterN = Cur_Param[FILTER_N];
    ProductPara.bFlashWEn = FLASH_WR_DISABLE;
}

