#include "BSP.h"
#include "eeprom.h"

uint8_t Onlineflg;

//const
uint8_t SensorSoftVersion[8] = {0x07, 'S', 'V', '3', '.', '1', '.', '4'};      //软件版本
//const
uint8_t Cap_User_Default_Param[PRO_DEFAULT_LEN] =
{
	0x01,								        //参数初始化标志位
        
 	0x41,								        //设备地址 ori 0x41
	0x03,								        //波特率(9600)
	0x03,								        //奇偶校验(无校验)
	0x01,								        //滤波系数
	0x01,								        //自动发送周期
	0x3F, 0x80, 0x00, 0x00,			//修正系数K
	0x3F, 0x80, 0x00, 0x00,			//修正系数B
	0x01,								        //补偿使能
	0x00,								        //是否冻结设备      

	0x00, 0x78,						    //传感器长度(0.1mm)30DE4
	0x00, 0x03, 0x0D, 0xE4,		//零点电容值 0x00, 0x0C, 0x48, 0x5D,
	0x00, 0x05, 0x61, 0x32,		//满量程电容值  //ori 0x00, 0x0D, 0xCF, 0x63,
	0x00, 0x00, 0x00, 0x00,		//零点DA值
	0x45, 0x3B, 0x80, 0x00,		//满度DA值
	
	0x00, 0x00, 0x00, 0x00,		//出厂量程
	0x00, 0x00, 0x00, 0x00,		//出厂零点电容值
	0x00, 0x00, 0x00, 0x00,		//出厂满量程电容值
	0x3F, 0x80, 0x00, 0x00,		//温度系数K
	0x00, 0x00, 0x00, 0x00,		//温度系数B
	0x3F, 0x80, 0x00, 0x00,		//温度系数K
	0x00, 0x00, 0x00, 0x00,		//温度系数B
  0x00, 0x00, 0x00, 0x00,   //液位DA最大值
	0x03,                     //液位比较时间间隔s  2 3 4 5
  0x04,                     //燃料默认柴油
  0x01,                      //油箱形状默认长方体
  0x05,0xDC,                 //油箱长1500
  0x02,0x58,                 //油箱宽600
  0x01,0xF4,                 //油箱高500
  0x00,0x03,                 //油箱厚3
  0x00,0x5A,                 //加油持续时间s
  0x01,0x09,                 //加油量阀值0.1L
  0x00,0x5A,                 //漏油持续时间s
  0x01,0x09,                  //漏油量阀值0.1L
};

uint8_t Cur_Param[PRO_DEFAULT_LEN] = {0};                   //系统当前参数
uint8_t Calib_Param[CALIB_DEFAULT_LEN] = {0};                //标定参数
uint8_t CALIB_Data[2] = {0};

uint8_t CALIB_Table[CALIN_HEIGH_TABLE_NUM];                  //油量标定数组

ProductParaTypeDef ProductPara;                         //产品参数
RunVarTypeDef RunVar;                                  //运行变量
UartCommTypeDef UartComm;                                //串口参数
UserParamTypeDef UserParam;                             //用户参数


//初始化IIC接口
void AT24CXX_Init(void)
{
//    IIC_Init();
}

//在AT24CXX指定地址读出一个数据
//ReadAddr:开始读数的地址
//返回值  :读到的数据
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
//        IIC_Send_Byte(DriverAddr);	   //发送写命令
//        IIC_Wait_Ack();
//        IIC_Send_Byte(ReadAddr >> 8);  //发送高地址
//        IIC_Wait_Ack();
//    }
//    else
//    { 
//        IIC_Send_Byte(DriverAddr + ((ReadAddr / 256) << 1));        //发送器件地址0XA0,写数据
//    }
//	
//    IIC_Wait_Ack();
//
//    IIC_Send_Byte(ReadAddr % 256);     //发送低地址
//
//    IIC_Wait_Ack();
//
//    IIC_Start();
//
//    IIC_Send_Byte(0XA1);           //进入接收模式
//
//    IIC_Wait_Ack();
//
//    temp = IIC_Read_Byte(0);
//
//    IIC_Stop();//产生一个停止条件
//
//    OS_CRITICAL_EXIT();

    return 0;//temp;
}

//在AT24CXX指定地址写入一个数据
//WriteAddr  :写入数据的目的地址
//DataToWrite:要写入的数据
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
//        IIC_Send_Byte(DriverAddr);	    //发送写命令
//        IIC_Wait_Ack();
//        IIC_Send_Byte(WriteAddr >> 8);  //发送高地址
//    }
//    else
//    {
//        IIC_Send_Byte(DriverAddr + ((WriteAddr / 256) << 1));        //发送器件地址0XA0,写数据
//    }
//
//    IIC_Wait_Ack();
//
//    IIC_Send_Byte(WriteAddr % 256);     //发送低地址
//    IIC_Wait_Ack();
//    IIC_Send_Byte(DataToWrite);     //发送字节
//    IIC_Wait_Ack();
//    IIC_Stop();//产生一个停止条件
//    Delay_Ms(2);
//
//    OS_CRITICAL_EXIT();
}


//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void I2C1_ReadNBytes(uint16_t DriverAddr, uint16_t ReadAddr, uint16_t NumToRead, uint8_t *pBuffer)
{
//    while (NumToRead)
//    {
//        *pBuffer++ = I2C1_ReadByte(DriverAddr, ReadAddr++);
//        NumToRead--;
//    }
}

//在AT24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
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

//在AT24CXX指定地址写入三份指定个数数据
//WriteAddr  :写入数据的目的地址
//DataToWrite:要写入的数据
//pBuffer   :数据数组首地址
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

//在AT24CXX初始化
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
        UserParam.FilterCycle = 1;      //实时滤波1s
        break;

    case 2:	
        UserParam.FilterBufMax = 18;    
        UserParam.FilterCycle = 180;    //平滑滤波180s
        break;

    case 3:	
        UserParam.FilterBufMax = 96;    //平稳滤波960s
        UserParam.FilterCycle = 960;
        break;

    default:
        UserParam.FilterBufMax = 18;    
        UserParam.FilterCycle = 180;    //实时滤波1s
        break;
    }
}

//检查用户参数
void Check_User_Backup(void)
{
    uint32_t i = 0, Eerrno = 0;
    uint8_t bak1 = 0, bak2 = 0;
    
    for (i = 0; i < PRO_DEFAULT_LEN; i++)
    {
        Eerrno = 0;
        //读出备份1和备份2数据
//        bak1 = I2C1_ReadByte(EEPROM_ADDRESS, i + USER_DEFAULT_PARA_BAK1);
//        bak2 = I2C1_ReadByte(EEPROM_ADDRESS, i + USER_DEFAULT_PARA_BAK2);
		Eeprom_ReadNBytes(i + USER_DEFAULT_PARA_BAK1, &bak1, 1);
		Eeprom_ReadNBytes(i + USER_DEFAULT_PARA_BAK2, &bak2, 1);
		
//	  rt_kprintf("bak1 %x %x\n" ,bak1,Cur_Param[i]);
//		rt_kprintf("bak2 %x %x\n" ,bak2,Cur_Param[i]);
        //当前参数与备份1比较
        if (Cur_Param[i] != bak1)
            Eerrno |= 0x01;
        //当前参数与备份2比较
        if (Cur_Param[i] != bak2)
            Eerrno |= 0x02;
        //备份1与备份2比较
        if (bak1 != bak2)
            Eerrno |= 0x04;
        //无错误
        if(0x00 == Eerrno)
            continue;
        //处理错误类型并纠正
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

//检查标定参数
void Check_Calib_Backup(void)
{
    uint32_t i = 0, Eerrno = 0;
    uint8_t bak1 = 0, bak2 = 0;

    for(i = 0; i < CALIB_DEFAULT_LEN; i++)
    {
      Eerrno = 0;
      //读出备份1和备份2数据
//     bak1 = I2C1_ReadByte(EEPROM_ADDRESS, i + CALIB_DEFAULT_PARA_BAK1);
//      bak2 = I2C1_ReadByte(EEPROM_ADDRESS, i + CALIB_DEFAULT_PARA_BAK2);
	  Eeprom_ReadNBytes(i + CALIB_DEFAULT_PARA_BAK1, &bak1, 1);
	  Eeprom_ReadNBytes(i + CALIB_DEFAULT_PARA_BAK2, &bak2, 1);
      //当前参数与备份1比较
      if (Calib_Param[i] != bak1)
        Eerrno |= 0x01;
      //当前参数与备份2比较
      if (Calib_Param[i] != bak2)
        Eerrno |= 0x02;
      //备份1与备份2比较
      if (bak1 != bak2)
        Eerrno |= 0x04;
      //无错误
      if(0x00 == Eerrno)
        continue;
      //处理错误类型并纠正  
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

//读取设备参数
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
    if(Cap_User_Default_Param[0] != ParaInitFlag)	//若EEPROM无数据，写入出厂值
    {
      //写入三份缺省值               //CALIB_DEFAULT_LEN
     // I2C1_WNBytesMul3T(EEPROM_ADDRESS, RUN_ADDR_BASE, USER_DEFAULT_LEN, Cap_User_Default_Param);//写基本参数，标定参数不写
	  Eeprom_WriteNBytes(RUN_ADDR_BASE, Cap_User_Default_Param, PRO_DEFAULT_LEN);// ori USER_DEFAULT_LEN PRO_DEFAULT_LEN
	  
	  //rt_kprintf("Cap_User_Default_Param\n");

	}
    //如果在线升级标志无效就擦除为默认值
    
	Eeprom_ReadNBytes(ONLINEUPGRADE, &ParaInitFlag, 1);
    if(0x0C != ParaInitFlag)
    {
        //I2C1_WriteByte(EEPROM_ADDRESS, ONLINEUPGRADE, 0x00);
		Eeprom_WriteNBytes(ONLINEUPGRADE, write_zero, 1);
    }
    //写入软件版本
    //I2C1_WriteNBytes(EEPROM_ADDRESS, SENSOR_SV, 8, SensorSoftVersion);
	Eeprom_WriteNBytes(SENSOR_SV,SensorSoftVersion,8);
    //读取用户参数
//    I2C1_ReadNBytes(EEPROM_ADDRESS, RUN_ADDR_BASE, PRO_DEFAULT_LEN, Cur_Param);
	Eeprom_ReadNBytes(RUN_ADDR_BASE, Cur_Param, PRO_DEFAULT_LEN);
//rt_kprintf("cap_Cur_Param %x %x %x\n",Cur_Param[0],Cur_Param[1],Cur_Param[2]);
    //读取油浮标定参数
//    I2C1_ReadNBytes(EEPROM_ADDRESS, CALIB_FLOATER_SCALE_1, CALIB_DEFAULT_LEN, Calib_Param);
	Eeprom_ReadNBytes(CALIB_FLOATER_SCALE_1, Calib_Param, CALIB_DEFAULT_LEN);
	
//    Check_User_Backup();
    Check_Calib_Backup();
    
    //读取标定温度 保留
//    I2C1_ReadNBytes(EEPROM_ADDRESS, CALIBTEMPER, 2, ptmp);
	Eeprom_ReadNBytes(CALIBTEMPER, ptmp, 2);
    ProductPara.CalibTemp = (ptmp[0] << 8)|ptmp[1];
    //读取油量量程
//    I2C1_ReadNBytes(EEPROM_ADDRESS, CALIBOILRAG, 4, ptmp);
	Eeprom_ReadNBytes(CALIBOILRAG, ptmp, 4);
    ProductPara.OilRange = 0;
    for(di = 0; di < 4; di++)
    {
      ProductPara.OilRange = (ProductPara.OilRange << 8) + ptmp[di];
    }
        
    //从机地址范围
   /* if((0x41 > Cur_Param[SLAVE_ADDR])||(0x42 < Cur_Param[SLAVE_ADDR]))
    {
      Cur_Param[SLAVE_ADDR] = 0x41;
    }*/
    //波特率范围
    if((0x01 > Cur_Param[BAUDRATE])||(0x07 < Cur_Param[BAUDRATE]))
    {
      Cur_Param[BAUDRATE] = 0x03;
    }
    //奇偶校验范围
    if((0x01 > Cur_Param[PARITY])||(0x03 < Cur_Param[PARITY]))
    {
      Cur_Param[PARITY] = 0x03;
    }
		
    //从机地址
    UartComm.SlaveAddr = Cur_Param[SLAVE_ADDR];
	//	rt_kprintf("UartComm.SlaveAddr %x %x\n",UartComm.SlaveAddr,Cur_Param[1]);
    //波特率
    UartComm.BaudRate = Cur_Param[BAUDRATE];
    //奇偶校验
    UartComm.Parity = Cur_Param[PARITY];
    //滤波等级
    UserParam.FilterLevel = Cur_Param[FILTER_LEVEL];
    //滤波切换
    Switch_Fiter(UserParam.FilterLevel);
    //自动上传周期
    ProductPara.AutoUpLoadTime = Cur_Param[AUTO_UPLOAD_TIME];
    //修正系数K
    UserParam.UserFlowK = HexToFloat(&Cur_Param[USER_FLOW_K]);
    //修正系数B
    UserParam.UserFlowB = HexToFloat(&Cur_Param[USER_FLOW_B]);
	
    //补偿使能
    ProductPara.CompenEn = Cur_Param[COMPENSATE];
    //是否冻结
    UartComm.Echo = Cur_Param[ECHOEN];

    //电容零点
    ProductPara.CapMin = ArrToHex(&Cur_Param[CAPMIN]);
    //电容满度
    ProductPara.CapMax = ArrToHex(&Cur_Param[CAPMAX]);
    //加油量阈值
    ProductPara.AddLiqCnt = (Cur_Param[ADDLIQLMT] << 8)|Cur_Param[ADDLIQLMT +1];
    //二次加油阈值
    ProductPara.RestLiqCnt = ProductPara.AddLiqCnt / 2;
    //加油时间阈值
    ProductPara.AddLiqTime = (Cur_Param[ADDLIQTIME] << 8)|Cur_Param[ADDLIQTIME +1];
    //漏油量阈值
    ProductPara.SubLiqCnt = (Cur_Param[SUBLIQLMT] << 8)|Cur_Param[SUBLIQLMT +1];
    //漏油时间阈值
    ProductPara.SubLiqTime = (Cur_Param[SUBLIQTIME] << 8)|Cur_Param[SUBLIQTIME +1];
    //DA零点
    ProductPara.DAMin = HexToFloat(&Cur_Param[DAMIN]);
    //DA满度
    ProductPara.DAMax = HexToFloat(&Cur_Param[DAMAX]);
    //出厂电容量程
    ProductPara.Def_Range = HexToFloat(&Cur_Param[DEF_RANGE]);
    //出厂电容零点
    ProductPara.Def_CapMin = HexToFloat(&Cur_Param[DEF_CAPMIN]);
    //出厂电容满度
    ProductPara.Def_CapMax = HexToFloat(&Cur_Param[DEF_CAPMAX]);

    //温度1系数K
    ProductPara.Temp_K = HexToFloat(&Cur_Param[TEMP_K]);
    //温度1系数B
    ProductPara.Temp_B = HexToFloat(&Cur_Param[TEMP_B]);
    //温度2系数K
    ProductPara.Temp2_K = HexToFloat(&Cur_Param[TEMP2_K]);
    //温度2系数B
    ProductPara.Temp2_B = HexToFloat(&Cur_Param[TEMP2_B]);
    //液位比较时间间隔    
    if((0x02 > Cur_Param[LIQUIDCMPTC]) || (0x05 < Cur_Param[LIQUIDCMPTC]))
    {
      Cur_Param[LIQUIDCMPTC] = 0x03;
//      I2C1_WNBytesMul3T(EEPROM_ADDRESS, LIQUIDCMPTC, 1, &Cur_Param[LIQUIDCMPTC]);
	  Eeprom_WriteNBytes(LIQUIDCMPTC, &Cur_Param[LIQUIDCMPTC], 1);
	}
    ProductPara.LiquidCmpTc = Cur_Param[LIQUIDCMPTC];
    
    
    ProductPara.FuelType = Cur_Param[FUELTYPE];                                 //燃料类型
    ProductPara.BoxShape = Cur_Param[BOXSHAPE];                                 //油箱形状
    ProductPara.BoxPara[0] = (Cur_Param[BOXPARA0] << 8)|Cur_Param[BOXPARA0 +1]; //长
    ProductPara.BoxPara[1] = (Cur_Param[BOXPARA1] << 8)|Cur_Param[BOXPARA1 +1]; //宽
    ProductPara.BoxPara[2] = (Cur_Param[BOXPARA2] << 8)|Cur_Param[BOXPARA2 +1]; //高
    ProductPara.BoxPara[3] = (Cur_Param[BOXPARA3] << 8)|Cur_Param[BOXPARA3 +1]; //厚
    /*
    ProductPara.BottomArea = (ProductPara.BoxPara[0] - 2*ProductPara.BoxPara[3])
                                * (ProductPara.BoxPara[1] - 2*ProductPara.BoxPara[3]) / 1000000.0f;//平方米
    ProductPara.AddMapHeight = ProductPara.AddLiqCnt * 100.0f / ProductPara.AddLiqTime / ProductPara.BottomArea;//设置的阈值反映到的高度*1000mm
    ProductPara.SubMapHeight = ProductPara.SubLiqCnt * 100.0f / ProductPara.SubLiqTime / ProductPara.BottomArea;
    */
    //液位最大DA  保留    
    ProductPara.LiquitHeightforDAMax = HexToFloat(&Cur_Param[HEIGHTDAMAX]);
    ProductPara.SensorLen = (Cur_Param[RANGE] << 8) + Cur_Param[RANGE + 1];     //传感器长度
    ProductPara.Range = ProductPara.SensorLen - 330;                            //传感器量程=长度-盲区（上下盲区和固定33mm）
		rt_kprintf("rd_eep senslen&range %x %x\n",ProductPara.SensorLen,ProductPara.Range);

    //油浮标定参数
    for(di=0; di < 8; di++)
    {
      //I2C1_ReadNBytes(EEPROM_ADDRESS, CALIB_FLOATER_DA_1 + 2 * di, 2, CALIB_Data);
	  Eeprom_ReadNBytes(CALIB_FLOATER_DA_1 + 2 * di, CALIB_Data, 2);
      ProductPara.Floater.DACode[di] = CALIB_Data[1] * 256 + CALIB_Data[0];
//      I2C1_ReadNBytes(EEPROM_ADDRESS, CALIB_FLOATER_SCALE_1 + 2 * di, 2, CALIB_Data);
	  Eeprom_ReadNBytes(CALIB_FLOATER_SCALE_1 + 2 * di, CALIB_Data, 2);
      ProductPara.Floater.Scale[di] = CALIB_Data[1] * 256 + CALIB_Data[0];
    }
    //油量标定数组    
//    I2C1_ReadNBytes(EEPROM_ADDRESS, CALIB_HEIGH_RELATIONSHIP, CALIN_HEIGH_TABLE_NUM, CALIB_Table);
	Eeprom_ReadNBytes(CALIB_HEIGH_RELATIONSHIP, CALIB_Table, CALIN_HEIGH_TABLE_NUM);

	//电容量程
    ProductPara.CapRange = ProductPara.CapMax - ProductPara.CapMin;
    //油量初始化标志
    RunVar.OilInit = 1;
    //Flash写使能禁止
    //UserParam.FilterM = Cur_Param[FILTER_M];
    //UserParam.FilterN = Cur_Param[FILTER_N];
    ProductPara.bFlashWEn = FLASH_WR_DISABLE;
}

