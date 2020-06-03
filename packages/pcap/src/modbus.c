#include "bsp.h"
#include "type.h"

//#include "os_cfg_app.h"

//#define MAX_USART1_DATA_LEN   210   //发送接收最长的数据 (该值小于256）

extern uint8_t Onlineflg;                                                            //在线升级标志
extern uint8_t UART1_RXBuff[USART1_MAX_DATALEN];                                   //串口接收缓存
extern uint8_t const SensorSoftVersion[8];                                         //软件版本
extern uint8_t const Cap_User_Default_Param[PRO_DEFAULT_LEN];                          //用户缺省参数
extern BitAction Add_Sub_Flag;
extern BitAction Calibrate_Flag;
extern struct rt_event nbtempsend_event;


//extern UserParamTypeDef UserParam;    //用户参数
//extern RunVarTypeDef RunVar; 
//extern ProductParaTypeDef ProductPara;                         //产品参数

uint8_t SendLen;                                                                     //发送数据长度
uint8_t SendBuf[DATAPACKET_LEN];                                                     //存储从机响应消息帧

uint32_t dwEepromHeightADValue;                                                      //油量数组液位高度
uint32_t dwEepromOilValue;                                                           //油量数组油量
uint32_t dwUpEepromHeightADValue;                                                    //上一个油量数组液位高度
uint32_t dwUpEepromOilValue;                                                         //上一个油量数组油量
uint32_t dwTankOilValue;                                                             //当前油箱油量
float dwTable_k;                                                               //油量关系斜率
extern rt_sem_t cap_sem;
extern struct rt_event cap_event;
extern rt_device_t tempncap_serial_dev;
extern rt_mutex_t mindraytask;

//**************************************************************************************************
// 名称         	: MBASCII_GetSlaveAddr()
// 创建日期   	        : 2015-10-29
// 作者         	: wang
// 功能         	:获得设备地址
// 输入参数     	:接收的缓冲数据(u8 *u8Msg)
// 输出参数     	: 设备地址
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

uint32_t MBASCII_GetSlaveAddr(uint8_t *u8Msg)
{
    return(u8Msg[0]);
}

//**************************************************************************************************
// 名称         	: MBASCII_GetFunCode()
// 创建日期   	        : 2015-10-29
// 作者         	: wang
// 功能         	:获得功能码
// 输入参数     	:接收的缓冲数据(u8 *u8Msg)
// 输出参数     	: 功能码
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

uint32_t MBASCII_GetFunCode(uint8_t *u8Msg)
{
    return(u8Msg[1]);
}

//**************************************************************************************************
// 名称         	: MBASCII_SendMsg()
// 创建日期   	        : 2015-10-29
// 作者         	: wang
// 功能         	:发送指令
// 输入参数     	:发送缓冲数据(uint8_t *u8Msg),数据长度(uint32_t u8MsgLen)
// 输出参数     	: 设备地址
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

void MBASCII_SendMsg(uint8_t *u8Msg, uint32_t u8MsgLen)
{
//  Delay_Ms(39);
  rt_thread_mdelay(39);
//	rt_kprintf("MBASCII_SendMsg %s\n",*u8Msg);

  if((MB_ADDRESS_BROADCAST != MBASCII_GetSlaveAddr(u8Msg)) && (UartComm.Echo == ECHO_ENABLE))
  {
    MODBUS_ASCII_SendData( u8Msg, u8MsgLen );	
  }
}


//**************************************************************************************************
// 名称         	: MBASCII_SendErr()
// 创建日期     	: 2015-07-13
// 作者        	 	: 王小鑫
// 功能         	: 发送MODBUS 错误
// 输入参数     	: u8 ErrCode
// 输出参数     	: 无
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

void MBASCII_SendErr(uint8_t ErrCode)
{
  SendLen = 1;
  SendBuf[SendLen++] |= 0x80;
  SendBuf[SendLen++] = ErrCode;

  MBASCII_SendMsg(SendBuf,SendLen);
}

//**************************************************************************************************
// 名称         	: MBASCII_SendMsg_NoLimit()
// 创建日期   	        : 2015-10-29
// 作者         	: wang
// 功能         	:发送指令
// 输入参数     	:发送缓冲数据(uint8_t *u8Msg),数据长度(uint32_t u8MsgLen)
// 输出参数     	: 设备地址
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

void MBASCII_SendMsg_NoLimit(uint8_t *u8Msg, uint32_t u8MsgLen)
{
  rt_thread_mdelay(39);
  if (UartComm.Echo == ECHO_ENABLE)
  {
    MODBUS_ASCII_SendData( u8Msg, u8MsgLen );
  }
}

//**************************************************************************************************
// 名称         	: Read_LoadAndWeightTable()
// 创建日期     	: 2015-07-13
// 作者        	 	: 庄明群
// 修改人               : 庄明群
// 功能         	: 读取载重AD值和称重对应表
// 输入参数     	: 无
// 输出参数     	: 无
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:帧格式
//**************************************************************************************************

void Read_CALIBAndHeighTable(void)
{
  uint8_t i;
  uint8_t dwSendLen = 0;
  uint16_t dwPacketSum;//总包数
  uint16_t dwPacketNum;//读第几包数据
  uint8_t dwTable[200];
  dwPacketSum = UART1_RXBuff[7]*256 + UART1_RXBuff[8];
  dwPacketNum = UART1_RXBuff[9]*256 + UART1_RXBuff[10];

  dwTable[dwSendLen++] = (MBASCII_GetSlaveAddr(UART1_RXBuff)) ? UartComm.SlaveAddr : 0x00;
  dwTable[dwSendLen++] = MB_FUNC_READ_HOLDING_REGISTER;
  dwTable[dwSendLen++] = UART1_RXBuff[2];
  dwTable[dwSendLen++] = UART1_RXBuff[3];
  dwTable[dwSendLen++] = UART1_RXBuff[4];
  dwTable[dwSendLen++] = UART1_RXBuff[5];
  dwTable[dwSendLen++] = UART1_RXBuff[6];
  dwTable[dwSendLen++] = UART1_RXBuff[7];//总包数
  dwTable[dwSendLen++] = UART1_RXBuff[8];
  dwTable[dwSendLen++] = UART1_RXBuff[9];//第几包数
  dwTable[dwSendLen++] = UART1_RXBuff[10];

  if(0x05 >= dwPacketSum)//总包数
  {
    if(dwPacketNum < 0x05)
    {
      for(i = 0; i < 80; i++)
      {
	dwTable[dwSendLen++] = CALIB_Table[dwPacketNum * 80 + i];
      }
      MBASCII_SendMsg(dwTable,dwSendLen);
    }	
  }
}


//**************************************************************************************************
// 名称         	: MBASCII_Fun03()
// 创建日期     	: 2016-09-01
// 作者        	 	: 庄明群
// 功能         	: Modbus 03功能处理
// 输入参数     	: 无
// 输出参数     	: 无
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************
//3A 34 31 30 33   34 31 38 30  30 30 30 31  34 39 0D 0A
void MBASCII_Fun03(void)
{
  uint8_t i;
  uint8_t Addrflg;//寄存器地址标志 用于标识只读一个从机地址信息时忽略请求帧的从机地址
  uint8_t ReadAdrH;//寄存器地址高8位
  uint8_t ReadAdrL;//寄存器地址低8位
  uint16_t Data_Buf;//数据内容
  uint16_t RegisterNum = (uint16_t)UART1_RXBuff[4]*256 + UART1_RXBuff[5];//寄存器数量

  ReadAdrH = UART1_RXBuff[2];
  ReadAdrL = UART1_RXBuff[3];
        
  SendLen = 0;
  SendBuf[SendLen++] = (MBASCII_GetSlaveAddr(UART1_RXBuff)) ? UartComm.SlaveAddr : 0x00;
  SendBuf[SendLen++] = MB_FUNC_READ_HOLDING_REGISTER;//功能码
  SendBuf[SendLen++] = RegisterNum * 2;	//数据长度
        
  Addrflg = 0;
   
  //判断访问寄存器地址是否在有效范围内
  if(!(((ReadAdrL >= ASCII_HOLDING_REG_REGION1_BGEIN) && (ReadAdrL <= ASCII_HOLDING_REG_REGION1_END)
     && (ReadAdrL + RegisterNum <= (ASCII_HOLDING_REG_REGION1_END + 1))&&(ReadAdrH == UartComm.SlaveAddr))
     ||((ReadAdrL == ASCII_HOLDING_REG_REGION3_BGEIN) && (RegisterNum == 0x2A)
     && (RegisterNum*2 == UART1_RXBuff[6])&&(ReadAdrH == UartComm.SlaveAddr))
     ||((ASCII_HOLDING_REG_REGION1_BGEIN == ReadAdrL)&&(1 == RegisterNum)))||(0 == RegisterNum))
  {
    //返回非法数据地址
    MBASCII_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
    return;
  }

  //如果访问油量标定数组
  if(0x80 == ReadAdrL)
  {
    //读取油量对应表
    Read_CALIBAndHeighTable();
    return;
  }
        
  for(long k = 0; k <  RegisterNum; ReadAdrL++, k++)
  {
    switch(ReadAdrL)
    {
    case 0x30://从机地址
      Addrflg = 1;
      Data_Buf = UartComm.SlaveAddr;
    break;
            
    case 0x31://波特率
      Data_Buf = UartComm.BaudRate;
    break;
            
    case 0x32://奇偶校验
      Data_Buf = UartComm.Parity;
    break;
            
    case 0x33:
      Data_Buf = 0;
    break;
    
    case 0x34://补偿使能
      Data_Buf = ProductPara.CompenEn;
    break;    
            
    case 0x35://滤波等级
      Data_Buf = UserParam.FilterLevel;
    break;
            
    case 0x36://自动上传周期
      Data_Buf = ProductPara.AutoUpLoadTime;
    break;
            
    case 0x37://电容修正系数K
      Data_Buf = (uint32_t)(UserParam.UserFlowK * 100);
    break;
            
    case 0x38://电容修正系数B
      Data_Buf = (uint32_t)(UserParam.UserFlowB * 100);
    break;
    
    case 0x39:
      Data_Buf = UserParam.FilterM;
      break;

    case 0x3A:
      Data_Buf = UserParam.FilterN;
      break;      
            
    case 0x3F://油杆长度
      Data_Buf = ProductPara.SensorLen;
    break;
            
    case 0x40://液位比较时间间隔
      Data_Buf = ProductPara.LiquidCmpTc;
    break;
            
    case 0x41://燃油类型
      Data_Buf = ProductPara.FuelType;
    break;
            
    case 0x42://油箱形状
      Data_Buf = ProductPara.BoxShape;
    break;
            
    case 0x43://油箱长度
      Data_Buf = ProductPara.BoxPara[0];
    break;
            
    case 0x44://油箱宽度
      Data_Buf = ProductPara.BoxPara[1];
    break;
            
    case 0x45://油箱高度
      Data_Buf = ProductPara.BoxPara[2];
    break;
            
    case 0x46://油箱壁厚 保留
      Data_Buf = ProductPara.BoxPara[3];
    break;
            
    case 0x47://加油时间阈值
      Data_Buf = ProductPara.AddLiqTime;
    break;
            
    case 0x48://加油量阈值
      Data_Buf = ProductPara.AddLiqCnt;
    break;
            
    case 0x49://漏油时间阈值
      Data_Buf = ProductPara.SubLiqTime;
    break;
            
    case 0x4A://漏油量阈值
      Data_Buf = ProductPara.SubLiqCnt;
    break;
            
    default:
      Data_Buf = 0;
    break;
    }
          
    for(i = 2; i > 0; i--)
    {
      SendBuf[SendLen++] = (uint8_t)(Data_Buf >> ((i - 1) * 8));
    }
  }
  //只读一个从机地址消息的响应      
  if((1 == Addrflg)&&(2 == SendBuf[2]))
  {
    MODBUS_ASCII_SendData(SendBuf,SendLen);
  }
  //读多个消息或其它内容的响应
  else
  {
    MBASCII_SendMsg(SendBuf,SendLen);
  }
}

//**************************************************************************************************
// 名称         	: MBASCII_Fun04()
// 创建日期     	: 2016-09-01
// 作者        	 	: 庄明群
// 功能         	: Modbus 04功能处理
// 输入参数     	: 无
// 输出参数     	: 无
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************
BitAction Read_Add_Sub_FLag = Bit_RESET;                                        //读数据标识
void MBASCII_Fun04(void)
{
    uint8_t i;
    uint8_t ReadAdrH;//寄存器地址高8位
    uint8_t ReadAdrL;//寄存器地址低8位
    uint32_t Data_Buf;//数据内容
  	uint16_t RegisterNum = (uint16_t)UART1_RXBuff[4]*256 + UART1_RXBuff[5];//寄存器数量
    ReadAdrH = UART1_RXBuff[2];
    ReadAdrL = UART1_RXBuff[3];
    SendLen = 0;
    SendBuf[SendLen++] = (MBASCII_GetSlaveAddr(UART1_RXBuff)) ? UartComm.SlaveAddr : 0x00;//从机地址
    SendBuf[SendLen++] = MB_FUNC_READ_INPUT_REGISTER;//功能码
    SendBuf[SendLen++] = RegisterNum * 2;//数据长度
    rt_kprintf("F4 %x %x %x",ReadAdrH,ReadAdrL,RegisterNum);

    //如果读取范围溢出
    if(!(((ReadAdrL <= ASCII_INPUT_REG_REGION1_END)
    && ((ReadAdrL + RegisterNum) <= (ASCII_INPUT_REG_REGION1_END + 2))))
    ||(0 == RegisterNum)||(0 != (RegisterNum & 0x01))|| (0 != (ReadAdrL & 0x01))||(ReadAdrH != UartComm.SlaveAddr))
    {
        //返回非法访问地址
        
        MBASCII_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
       // return;
    }

    for(long k = 0; k < RegisterNum; ReadAdrL += 2, k += 2)
    {
        switch(ReadAdrL)
        {
            case 0x00:
                Data_Buf = RunVar.LiquidAD;
                if(Add_Sub_Flag == Bit_SET)
                {
                    Data_Buf |= 0x80000000;
                }
                else
                {
                    Data_Buf &= 0x7FFFFFFF;
                }              
                break;

            case 0x02:                                                              //液体温度                                                     
                Data_Buf = (uint32_t)((RunVar.TempInSensor + 273.15) * 10);
								
						  rt_kprintf("FO4_enc0x02 %d\n",RunVar.TempInSensor);
                break;

            case 0x04:                                                                //环境温度
                Data_Buf = (uint32_t)((RunVar.TempInAir + 273.15) * 10);
								
						rt_kprintf("FO4_enc0x04 %d\n",RunVar.TempInAir);
                break;

            case 0x06:                                                                //加油量
                if((Read_Add_Sub_FLag == Bit_RESET) && (Add_Sub_Flag == Bit_SET))
                {
                
								rt_kprintf("FO4_enc0x06 \n");
                    Read_Add_Sub_FLag = Bit_SET;                                //读取以后再保持1.6s
                }                                                                                                       
                Data_Buf = ProductPara.AddOil;
								
								rt_kprintf("FO4_enc0x06 %d\n",ProductPara.AddOil);
                break;

            case 0x08:                                                                //漏油量      
                Data_Buf = ProductPara.SubOil;
								
						rt_kprintf("FO4_enc0x08 %d\n",ProductPara.SubOil);
                break;

            case 0x0A:                                                                //油箱油量
                Data_Buf = RunVar.OilQuantity;
						rt_kprintf("FO4_enc0x0A %d\n",RunVar.OilQuantity);
                break;

            case 0x0C:                                                                //油量百分比
                Data_Buf = RunVar.LiquidPercent;
						rt_kprintf("FO4_enc0x0C %d\n",RunVar.LiquidPercent);
                break;

            case 0x0E:                                                                //液位高度(0.1mm)
                Data_Buf =RunVar.LiquidHeight;							
						rt_kprintf("FO4_enc0x0E %d\n",RunVar.LiquidHeight);
                break;

            default:  
                Data_Buf = 0;								
								rt_kprintf("FO4_encData_Buf %d\n",Data_Buf);
                break;
        } 
        for(i = 4; i > 0; i--)
        {
            SendBuf[SendLen++] = (uint8_t)(Data_Buf >> ((i - 1) * 8));
        }
    }
    MBASCII_SendMsg(SendBuf,SendLen);
}
void send_capto_pc(void *p_arg)
{
		uint8_t i;
		//uint8_t ReadAdrH;//寄存器地址高8位
		uint8_t ReadAdrL;//寄存器地址低8位
		uint32_t Data_Buf;//liq_height=0x00000486;//数据内容
		uint8_t bak1 = 0;		
//		if (mindraytask == RT_NULL)
//		{
//			/* create AT socket lock */
//			mindraytask = rt_mutex_create("mindraytask", RT_IPC_FLAG_FIFO);
//			if (mindraytask == RT_NULL)
//			{
//				rt_kprintf("No memory for mindraytask allocation lock!\n");
//				return ;
//			}
//		}
//  	uint16_t RegisterNum = 7;//(uint16_t)UART1_RXBuff[4]*256 + UART1_RXBuff[5];//寄存器数量
	//	ReadAdrH = UART1_RXBuff[2];
		ReadAdrL = 0;//UART1_RXBuff[3];
		SendLen = 0;   
//		SendBuf[SendLen++] = (MBASCII_GetSlaveAddr(UART1_RXBuff)) ? UartComm.SlaveAddr : 0x00;//从机地址
	  Eeprom_ReadNBytes(0x01, &bak1, 1);//从机地址
	  SendBuf[SendLen++] = bak1;
    SendBuf[SendLen++] = MB_FUNC_READ_INPUT_REGISTER;//功能码
		SendBuf[SendLen++] = 0x14;//数据长度
		//rt_enter_critical();
	while (1)
	{
	  
		rt_mutex_take(mindraytask, RT_WAITING_FOREVER);
		rt_enter_critical();
		for(long k = 0; k < 10; ReadAdrL += 2, k += 2)
		{
			 #if 1
				switch(ReadAdrL)
				{
						case 0x00:																	//液位高度(0.1mm)
								Data_Buf = RunVar.LiquidHeight;							
						rt_kprintf("LiquidHeight %d\n",RunVar.LiquidHeight);
								break;		

						case 0x02:	  																															//环境温度
								//Data_Buf = (uint32_t)((RunVar.TempInAir + 273.15) * 10);
								Data_Buf = 0;
//						    rt_kprintf("TempInAir %d\n",Data_Buf);
								break;		
						case 0x04:	//ok																															//液位高度(0.1mm)
								Data_Buf =ProductPara.FuelType;							
//						rt_kprintf("FuelType %d\n",Data_Buf);
								break;

//						case 0x04:	   //																															//环境温度
//								//Data_Buf = (uint32_t)((RunVar.TempInAir + 273.15) * 10);
//								Data_Buf = (31.2 + 273.15) * 10;
//						    rt_kprintf("TempInAir %d\n",Data_Buf);
//								break;
						case 0x06:	 	 																											//环境温度
									//Data_Buf = (((float)RunVar.TempInAir/100) + 273.15)*10;
									Data_Buf = RunVar.TempInAir;
									//Data_Buf = (31.2 + 273.15) * 10;
//									rt_kprintf("TempInAir %d\n",Data_Buf);
								break;
						case 0x08:																													//环境温度
									Data_Buf = RunVar.RawCap;
//								rt_kprintf("TempInAir %d\n",Data_Buf);
								break;

						default:	
						//	Data_Buf = RunVar.RawCap;//SecFilStr.EverySecCap;//UserParam.PCap_Filter;//SecFilStr.EverySecCap;
							//rt_kprintf("RawCap %d\n",Data_Buf);
								break;
				} 
				for(i = 4; i > 0; i--)
				{
						SendBuf[SendLen++] = (uint8_t)(Data_Buf >> ((i - 1) * 8));
				}
				#endif
		}
		//rt_exit_critical();
		MBASCII_SendMsg(SendBuf,SendLen);
		rt_thread_mdelay(100);
		rt_exit_critical();
//		rt_event_send(&nbtempsend_event,9); 
		rt_mutex_release(mindraytask);//frank cl
		rt_kprintf("send_mindray_pc \n");
//		rt_event_send(&nbtempsend_event,9); 	//check frank
		//rt_thread_mdelay(500);
  }		
}

//**************************************************************************************************
// 名称         	: MBASCII_Fun05()
// 创建日期     	: 2016-09-01
// 作者        	 	: 庄明群
// 功能         	: Modbus 05功能处理
// 输入参数     	: 无
// 输出参数     	: 无
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

void MBASCII_Fun05(void)
{
  uint8_t ReadAdrH;//寄存器高8位地址
  uint8_t ReadAdrL;//寄存器低8位地址
  
  ReadAdrH = UART1_RXBuff[2];
  ReadAdrL = UART1_RXBuff[3];
  
  SendLen = 0;
  SendBuf[SendLen++] = (MBASCII_GetSlaveAddr(UART1_RXBuff)) ? UartComm.SlaveAddr : 0x00;
  SendBuf[SendLen++] = MB_FUNC_WRITE_SINGLE_COIL;
  if((ReadAdrL < ASCII_SINGLE_COIL_ADDR_BGEIN) || (ReadAdrL > ASCII_SINGLE_COIL_ADDR_END) || (ReadAdrH != UartComm.SlaveAddr))
  { rt_kprintf("Fun050x02\n");
    MBASCII_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
    return;
  }
  if((ReadAdrL != ASCII_SINGLE_COIL_ADDR_END) && (ProductPara.bFlashWEn == FLASH_WR_DISABLE))
  {rt_kprintf("Fun050x04\n");
    MBASCII_SendErr(MB_EX_SLAVE_DEVICE_FAILURE);
    return;
  }
  if(!(((UART1_RXBuff[4] == 0xFF) && (UART1_RXBuff[5] == 0x00))
            || ((UART1_RXBuff[4] == 0x00) && (UART1_RXBuff[5] == 0x00))))
  {rt_kprintf("Fun050x03\n");
    MBASCII_SendErr(MB_EX_ILLEGAL_DATA_VALUE);
    return;
  }
  
  switch(ReadAdrL)
  {              //3a 34 31 30 35 34 31 35 30 46 46 30 30 32 41 d a
  case 0x50:   //3a 34 31 30 35  34 31 35 30  30 30 30 30  32 39 d a
    if((UART1_RXBuff[4] == 0x00) && (UART1_RXBuff[5] == 0x00))//标定低液位
    {
      Cur_Param[CAPMIN] = (unsigned char)(RunVar.RawCap >> 24);
      Cur_Param[CAPMIN+1] = (unsigned char)(RunVar.RawCap >> 16);
      Cur_Param[CAPMIN+2] =(unsigned char)(RunVar.RawCap >> 8);
      Cur_Param[CAPMIN+3] = (unsigned char)RunVar.RawCap;
      RunVar.CalState |= CalibrateLow;
			
		//	Eeprom_WriteNBytes(CAPMIN, Cap_User_Default_Param, PRO_DEFAULT_LEN);// ori USER_DEFAULT_LEN PRO_DEFAULT_LEN
			rt_kprintf("CapCalibrate %x %x %x %x %x\n",RunVar.RawCap,Cur_Param[CAPMIN],Cur_Param[CAPMIN+1],Cur_Param[CAPMIN+2],Cur_Param[CAPMIN+3]);
    }    //3a 34 31 30 35  34 31 35 30   46 46 30 30   32 41 d a
    else if((UART1_RXBuff[4] == 0xFF) && (UART1_RXBuff[5] == 0x00))//标定高液位
    {
      Cur_Param[CAPMAX] = (unsigned char)(RunVar.RawCap >> 24);
      Cur_Param[CAPMAX+1] = (unsigned char)(RunVar.RawCap >> 16);
      Cur_Param[CAPMAX+2] =(unsigned char)(RunVar.RawCap >> 8);
      Cur_Param[CAPMAX+3] = (unsigned char)RunVar.RawCap;
      RunVar.CalState |= CalibrateHeight;
    }
    CapCalibrate();
    break;
    
  case 0x51:
    if ((UART1_RXBuff[4] == 0xFF) && (UART1_RXBuff[5] == 0x00))
    {
      Cur_Param[0] = Cap_User_Default_Param[0] + 1;//恢复出厂设置
//      I2C1_WNBytesMul3T(EEPROM_ADDRESS, RUN_ADDR_BASE, 1, Cur_Param);
			Eeprom_WriteNBytes(RUN_ADDR_BASE, Cur_Param, 1);
//#if IWDG_EN > 0
//            IWDG_ReloadCounter();
//#endif
            Rd_Dev_Param();
//#if IWDG_EN > 0
//            IWDG_ReloadCounter();
//#endif
    }
    break;
    
  case 0x52:
    if ((UART1_RXBuff[4] == 0xFF) && (UART1_RXBuff[5] == 0x00))
    {
      UartComm.Echo = ECHO_ENABLE;//冻结设备
    }
    else
    {
      UartComm.Echo = ECHO_DISABLE;//解冻设备
    }
//    I2C1_WNBytesMul3T(EEPROM_ADDRESS, ECHOEN, 1, &UartComm.Echo);
		Eeprom_WriteNBytes(ECHOEN, &UartComm.Echo, 1);
    break;
    
  case 0x53:
    if ((UART1_RXBuff[4] == 0xFF) && (UART1_RXBuff[5] == 0x00))
    {
      ProductPara.bFlashWEn = FLASH_WR_ENABLE;//Flash写使能
    }
    else
    {
      ProductPara.bFlashWEn = FLASH_WR_DISABLE;//Flash写禁止
    }
    break;
    
  default:
    break;
  }
  //Modbus发送操作成功
  MBASCII_SendMsg(UART1_RXBuff, 6);
}

//**************************************************************************************************
// 名称         	: Write_CALIBAndHeighTable()
// 创建日期     	: 2015-07-13
// 作者        	 	: 庄明群 
// 修改人               : 庄明群
// 功能         	: 保存油高度AD值与油箱油量对应表
// 输入参数     	: 无
// 输出参数     	: 无
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	: 帧格式
//**************************************************************************************************

void Write_CALIBAndHeighTable(void)
{
  uint8_t opara[4],temp_data;
  uint16_t i,j;
  uint16_t PacketSum;//总包数
  uint16_t PacketNum;//包序号
  uint16_t RegisterNum;//寄存器数量
  uint32_t Oiltmp;
	
  PacketSum = UART1_RXBuff[7]*256 + UART1_RXBuff[8];//总包数
  PacketNum = UART1_RXBuff[9]*256 + UART1_RXBuff[10];//第几包
  RegisterNum = (uint16_t)UART1_RXBuff[4]*256 + UART1_RXBuff[5];//寄存器数量
  //如果总包数超过5包或者第一包寄存器数量小于10
  if((0x05 < PacketSum)||(0x000A > RegisterNum)&&(0 == PacketNum))
  {
    SendLen = 0;
    SendBuf[SendLen++] = (MBASCII_GetSlaveAddr(UART1_RXBuff)) ? UartComm.SlaveAddr : 0x00;
    SendBuf[SendLen++] = MB_FUNC_WRITE_MULTIPLE_REGISTERS;
    SendBuf[SendLen++] = ((uint16_t)UART1_RXBuff[4]*256 + UART1_RXBuff[5]) * 2;

    //返回非法数据地址错误
    MBASCII_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
    return;
  }

  //总包数且寄存器数量要小于42
  if((0x01 <= PacketSum)&&(0x05 >= PacketSum)&&(0x002A >= RegisterNum))
  {
    if(PacketNum < 0x05)
    {
      //传输一包数据到缓存
      for(i = 0; i < 80; i++)
      {
	CALIB_Table[PacketNum*80 + i] = UART1_RXBuff[11 + i];
      }

      //保存一包数据到EEPROM
//      I2C1_WriteNBytes(EEPROM_ADDRESS, CALIB_HEIGH_RELATIONSHIP + 80 * PacketNum, 80, &UART1_RXBuff[11]);
			Eeprom_WriteNBytes(CALIB_HEIGH_RELATIONSHIP + 80 * PacketNum,&UART1_RXBuff[11], 80); 								 
      RunVar.OilInit = 0;
      //是否是最后一包
      if(PacketSum == (PacketNum + 1))
      {
        Oiltmp = 0;
        ProductPara.OilRange = 0;
        for(i = 0; i < 4; i++)
        {
          Oiltmp = (Oiltmp << 8) + CALIB_Table[4 + i];
        }
                          
        for(i = 0; i < 4; i++)
        {
          j = 80 * PacketNum + 2 * RegisterNum - 16 + i;
          ProductPara.OilRange = (ProductPara.OilRange << 8) + CALIB_Table[j];
        }
        
        //油量量程  
        
        ProductPara.OilRange -= Oiltmp;
        
        //MinVarOil = (u32)(ProductPara.OilRange*0.01);
                          
        for(i = 4; i > 0; i--)
        {
          opara[4 - i] = (uint8_t)(ProductPara.OilRange >> ((i - 1) * 8));
        }
                          
//        I2C1_WriteNBytes(EEPROM_ADDRESS, CALIBOILRAG, 4, opara);
        Eeprom_WriteNBytes(CALIBOILRAG,opara, 4);                  
        //j = (5 - PacketSum)*80 + 80 - (RegisterNum - 2)*2;
        //补充油量标定数组的最后一组
        for(i = 0; i < 8; i++)
        {
          j = CALIB_HEIGH_RELATIONSHIP + 80 * PacketNum + 2 * RegisterNum - 4 + i;
//          I2C1_WriteByte(EEPROM_ADDRESS, j, 0xFF);
					temp_data = 0xFF;
					Eeprom_WriteNBytes(j,&temp_data, 1); 								 
        }
                          
        RunVar.OilInit = 1;
      }
      
      //UART1_RXBuff[6] = UART1_RXBuff[7];
      //UART1_RXBuff[7] = UART1_RXBuff[8];
      for(j = 0; j < 11; j++)
      {
	SendBuf[j] = UART1_RXBuff[j];
      }

      MBASCII_SendMsg(SendBuf,11);
      return;
    }
    /*else
      {
	SendLen = 0;
	SendBuf[SendLen++] = (MBASCII_GetSlaveAddr(UART1_RXBuff)) ? UartComm.SlaveAddr : 0x00;
	SendBuf[SendLen++] = MB_FUNC_WRITE_MULTIPLE_REGISTERS;
	SendBuf[SendLen++] = ((u16)UART1_RXBuff[4]*256 + UART1_RXBuff[5]) * 2;

	MBASCII_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
	return;
      }*/
		
  }
  else//否则发送访问地址溢出错误
  {
    SendLen = 0;
    SendBuf[SendLen++] = (MBASCII_GetSlaveAddr(UART1_RXBuff)) ? UartComm.SlaveAddr : 0x00;
    SendBuf[SendLen++] = MB_FUNC_WRITE_MULTIPLE_REGISTERS;
    //SendBuf[SendLen++] = ((u16)UART1_RXBuff[4]*256 + UART1_RXBuff[5]) * 2;
    SendBuf[SendLen++] = 0x02;

    MBASCII_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
    return;
  }
}


//**************************************************************************************************
// 名称         	: MBASCII_Fun10()
// 创建日期     	: 2016-09-01
// 作者        	 	: 庄明群
// 功能         	: Modbus 10功能处理
// 输入参数     	: 无
// 输出参数     	: 无
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************
//3a 34 31 31 30  34 31 33 46  30 30 30 31 30 32 30 30 37 38 42 34 d a 
void MBASCII_Fun10(void)
{
        uint8_t i = 0;
	uint8_t u8Temp[4];
        uint8_t ReadAdrH;
        uint8_t ReadAdrL;
        uint16_t u16Temp;
	uint16_t index = 0;
        uint8_t ComTime;
        
	uint16_t RegisterNum = (uint16_t)UART1_RXBuff[4]*256 + UART1_RXBuff[5];
	
	ReadAdrH = UART1_RXBuff[2];
        ReadAdrL = UART1_RXBuff[3];
        
	SendLen = 0;
    	SendBuf[SendLen++] = (MBASCII_GetSlaveAddr(UART1_RXBuff)) ? UartComm.SlaveAddr : 0x00;
    	SendBuf[SendLen++] = MB_FUNC_WRITE_MULTIPLE_REGISTERS;
    	SendBuf[SendLen++] = RegisterNum * 2;

        //如果读取范围溢出
    if (!(((ReadAdrL >= ASCII_MUL_REG_REGION1_BGEIN) && (ReadAdrL <= ASCII_MUL_REG_REGION1_END)
            && (ReadAdrL + RegisterNum <= (ASCII_MUL_REG_REGION1_END + 1)))
            //|| ((ReadAdrL >= ASCII_MUL_REG_REGION2_BGEIN) && (ReadAdrL <= ASCII_MUL_REG_REGION2_END)
            //    && (ReadAdrL + RegisterNum <= (ASCII_MUL_REG_REGION2_END + 1)))
            || ((ReadAdrL == ASCII_MUL_REG_REGION3_BGEIN))//&&(0x000A <= RegisterNum)&&(0x002A >= RegisterNum))
            || ((ReadAdrL >= ASCII_MUL_REG_EXTERN0_BEGIN)&&(ReadAdrL <= ASCII_MUL_REG_EXTERN0_END)
                &&(ReadAdrL + RegisterNum <= (ASCII_MUL_REG_EXTERN0_END + 1)))                                  //放过广播地址
            )||(0 == RegisterNum)||((RegisterNum * 2) != UART1_RXBuff[6])||((ReadAdrH != UartComm.SlaveAddr) && (ReadAdrH != 0)))
    	{
				MBASCII_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
			      // return;
			}
        ProductPara.bFlashWEn = FLASH_WR_ENABLE;
        //写油量标定数组
        if(0x80 == ReadAdrL)
        {
          rt_kprintf("写油量标定数组\n");
            //uprintf("receive OK!\r\n");
          Calibrate_Flag = Bit_SET;
          Write_CALIBAndHeighTable();
          return;
        }
        
	for (long k = 0; k < RegisterNum; ReadAdrL++, k++)
	{
          u16Temp = (uint16_t)UART1_RXBuff[7+index] * 256 + UART1_RXBuff[8+index];
          
          switch(ReadAdrL)
          {
          case 0x30://设备地址
            //if((UART1_RXBuff[8 + index] < 0x41)||(UART1_RXBuff[8 + index] > 0x42))
            //{
              //UartComm.SlaveAddr = 0x41;
            //}
            //else
            //{
              UartComm.SlaveAddr = UART1_RXBuff[8 + index];
            //}
//            I2C1_WNBytesMul3T(EEPROM_ADDRESS, SLAVE_ADDR, 1, &UartComm.SlaveAddr);
					
						Eeprom_WriteNBytes(SLAVE_ADDR, &UartComm.SlaveAddr, 1);
            break;
            
          case 0x31://波特率
            if((UART1_RXBuff[8 + index] < 0x01)||(UART1_RXBuff[8 + index] > 0x07))
            {
              UartComm.BaudRate = 0x03;
            }
            else
            {
              UartComm.BaudRate = UART1_RXBuff[8+index];
            }
//            I2C1_WNBytesMul3T(EEPROM_ADDRESS, BAUDRATE, 1, &UartComm.BaudRate);
						
						Eeprom_WriteNBytes(BAUDRATE, &UartComm.BaudRate, 1);
            break;
            
          case 0x32://奇偶校验
            if((UART1_RXBuff[8 + index] < 0x01)||(UART1_RXBuff[8 + index] > 0x03))
            {
              UartComm.Parity = 0x03;
            }
            else
            {
              UartComm.Parity = UART1_RXBuff[8+index];
            }
            
//            I2C1_WNBytesMul3T(EEPROM_ADDRESS, PARITY, 1, &UartComm.Parity);
						Eeprom_WriteNBytes(PARITY, &UartComm.Parity, 1);
            break;
            
          case 0x34://补偿使能
            if((0x00 < UART1_RXBuff[8+index]) && (0x03 > UART1_RXBuff[8+index]))
            {
              ProductPara.CompenEn = UART1_RXBuff[8+index];
            }
//            I2C1_WNBytesMul3T(EEPROM_ADDRESS, COMPENSATE, 1, &ProductPara.CompenEn);
						Eeprom_WriteNBytes(COMPENSATE, &ProductPara.CompenEn, 1);
            break;
            
          case 0x35://滤波系数
            if((0x00 < UART1_RXBuff[8+index]) && (0x04 > UART1_RXBuff[8+index]))
            {
              UserParam.FilterLevel = UART1_RXBuff[8+index];
              Switch_Fiter(UserParam.FilterLevel);
            }
//            I2C1_WNBytesMul3T(EEPROM_ADDRESS, FILTER_LEVEL, 1, &UserParam.FilterLevel);
						
						Eeprom_WriteNBytes(FILTER_LEVEL, &UserParam.FilterLevel, 1);
            break;
            
          case 0x36://自动上传周期
            ProductPara.AutoUpLoadTime = UART1_RXBuff[8+index];
            if ((ProductPara.AutoUpLoadTime < 0x01)||(ProductPara.AutoUpLoadTime > 0x04))
            {
              ProductPara.AutoUpLoadTime = 0x01;
            }
//            I2C1_WNBytesMul3T(EEPROM_ADDRESS, AUTO_UPLOAD_TIME, 1, &ProductPara.AutoUpLoadTime);
						Eeprom_WriteNBytes(AUTO_UPLOAD_TIME, &ProductPara.AutoUpLoadTime, 1);

						break;
            
          case 0x37://修正系数K
            UserParam.UserFlowK = u16Temp / 100.0;
            floatToHexArray(UserParam.UserFlowK, u8Temp, 4);
//            I2C1_WNBytesMul3T(EEPROM_ADDRESS, USER_FLOW_K, 4, u8Temp);
						
						Eeprom_WriteNBytes(USER_FLOW_K, u8Temp, 4);
            break;
            
          case 0x38://修正系数B
            UserParam.UserFlowB = u16Temp / 100.0;
            floatToHexArray(UserParam.UserFlowB, u8Temp, 4);
//            I2C1_WNBytesMul3T(EEPROM_ADDRESS, USER_FLOW_B, 4, u8Temp);
						Eeprom_WriteNBytes(USER_FLOW_B, u8Temp, 4);
						
            break;
            
          case 0x39:
            /*if(u16Temp != 0)
            {
                UserParam.FilterM = u16Temp;
                I2C1_WNBytesMul3T(EEPROM_ADDRESS, FILTER_M, 1, &UserParam.FilterM);
            }*/
            break;
            
          case 0x3A:
            /*if(u16Temp != 0)
            {
                UserParam.FilterN = u16Temp;
                I2C1_WNBytesMul3T(EEPROM_ADDRESS, FILTER_N, 1, &UserParam.FilterN);                
            }*/
            break;
            
          case 0x3F://油杆长度
          
					rt_kprintf("write_full_rang_before %x,%x\n",ProductPara.SensorLen,ProductPara.Range);
            ProductPara.SensorLen = u16Temp;
            ProductPara.Range = ProductPara.SensorLen - 330;     //传感器量程=长度-盲区（上下盲区和固定33mm）
           // I2C1_WNBytesMul3T(EEPROM_ADDRESS, RANGE, 2, &UART1_RXBuff[7+index]);
						Eeprom_WriteNBytes(RANGE, &UART1_RXBuff[7+index], 2);
					 rt_kprintf("write_full_rang_ok %x,%x\n",ProductPara.SensorLen,ProductPara.Range);

						break;
            
          case 0x40://液位比较时间间隔
            ComTime = UART1_RXBuff[8 + index];
            if((ComTime > 0x01) && (ComTime < 0x06))
            {
              ProductPara.LiquidCmpTc = ComTime;
//              I2C1_WNBytesMul3T(EEPROM_ADDRESS, LIQUIDCMPTC, 1, &ProductPara.LiquidCmpTc);
							Eeprom_WriteNBytes(LIQUIDCMPTC, &ProductPara.LiquidCmpTc, 1);

						}        
            break;
            
          case 0x41://油料类型
            ProductPara.FuelType = UART1_RXBuff[8 + index];
						rt_kprintf("FuelType_bef %x",ProductPara.FuelType);
            if((UART1_RXBuff[8 + index] < 0x01)||(UART1_RXBuff[8 + index] > 0x04))
            {
              ProductPara.FuelType = 0x01;
							
							rt_kprintf("FuelType_aft %x",ProductPara.FuelType);
            }
          //  I2C1_WNBytesMul3T(EEPROM_ADDRESS, FUELTYPE, 1, &ProductPara.FuelType);
						Eeprom_WriteNBytes(FUELTYPE, &ProductPara.FuelType, 1);
            break;
            
          case 0x42://油箱形状
            ProductPara.BoxShape = UART1_RXBuff[8 + index];
            if((UART1_RXBuff[8 + index] < 0x01)||(UART1_RXBuff[8 + index] > 0x05))
            {
              ProductPara.BoxShape = 0x01;
            }
//            I2C1_WNBytesMul3T(EEPROM_ADDRESS, BOXSHAPE, 1, &ProductPara.BoxShape);
						
						Eeprom_WriteNBytes(BOXSHAPE, &ProductPara.BoxShape, 1);
            break;
            
          case 0x43://油箱长度
            Cur_Param[BOXPARA0] = UART1_RXBuff[7+index];
            Cur_Param[BOXPARA0 + 1] = UART1_RXBuff[8+index];
            ProductPara.BoxPara[0] = u16Temp;
//            I2C1_WNBytesMul3T(EEPROM_ADDRESS, BOXPARA0, 2, &Cur_Param[BOXPARA0]);
						
						Eeprom_WriteNBytes(BOXPARA0, &Cur_Param[BOXPARA0], 2);
            break;
            
          case 0x44://油箱宽度
            Cur_Param[BOXPARA1] = UART1_RXBuff[7+index];
            Cur_Param[BOXPARA1 + 1] = UART1_RXBuff[8+index];
            ProductPara.BoxPara[1] = u16Temp;
//            I2C1_WNBytesMul3T(EEPROM_ADDRESS, BOXPARA1, 2, &Cur_Param[BOXPARA1]);
            
						Eeprom_WriteNBytes(BOXPARA1, &Cur_Param[BOXPARA1], 2);
            if(ComTime == 0)    //下发比较时间为0，则需要油箱截面积大小，根据截面积自动设置液位比较时间
            {
                if(ProductPara.BoxPara[0] * ProductPara.BoxPara[1] <= 750000)
                {
                    ProductPara.LiquidCmpTc = 3;
                }
                else if(ProductPara.BoxPara[0] * ProductPara.BoxPara[1] <= 1000000)
                {
                    ProductPara.LiquidCmpTc = 4;
                }
                else
                {
                    ProductPara.LiquidCmpTc = 5;
                }
                    
//                I2C1_WNBytesMul3T(EEPROM_ADDRESS, LIQUIDCMPTC, 1, &ProductPara.LiquidCmpTc);
								Eeprom_WriteNBytes(LIQUIDCMPTC,&ProductPara.LiquidCmpTc, 1);

						}
            break;
            
          case 0x45://油箱高度
            Cur_Param[BOXPARA2] = UART1_RXBuff[7+index];
            Cur_Param[BOXPARA2 + 1] = UART1_RXBuff[8+index];
            ProductPara.BoxPara[2] = u16Temp;
//            I2C1_WNBytesMul3T(EEPROM_ADDRESS, BOXPARA2, 2, &Cur_Param[BOXPARA2]);
						Eeprom_WriteNBytes(BOXPARA2,&Cur_Param[BOXPARA2], 2);
            break;
            
          case 0x46://油箱厚度
            Cur_Param[BOXPARA3] = UART1_RXBuff[7+index];
            Cur_Param[BOXPARA3 + 1] = UART1_RXBuff[8+index];
            ProductPara.BoxPara[3] = u16Temp;
//            I2C1_WNBytesMul3T(EEPROM_ADDRESS, BOXPARA3, 2, &Cur_Param[BOXPARA3]);
						
						Eeprom_WriteNBytes(BOXPARA3,&Cur_Param[BOXPARA3], 2);
            break;
            
          case 0x47://加油时间阀值，范围0-999
            if(1000 > u16Temp)
            {
              Cur_Param[ADDLIQTIME] = UART1_RXBuff[7+index];
              Cur_Param[ADDLIQTIME + 1] = UART1_RXBuff[8+index];
              ProductPara.AddLiqTime = u16Temp;
//              I2C1_WNBytesMul3T(EEPROM_ADDRESS, ADDLIQTIME, 2, &Cur_Param[ADDLIQTIME]);
							
							Eeprom_WriteNBytes(ADDLIQTIME,&Cur_Param[ADDLIQTIME], 2);
            }
            break;
            
          case 0x48://加油量阀值，范围1-9999
            if((0 < u16Temp)&&(10000 > u16Temp))
            {
              Cur_Param[ADDLIQLMT] = UART1_RXBuff[7+index];
              Cur_Param[ADDLIQLMT + 1] = UART1_RXBuff[8+index];
              ProductPara.AddLiqCnt = u16Temp;
//              I2C1_WNBytesMul3T(EEPROM_ADDRESS, ADDLIQLMT, 2, &Cur_Param[ADDLIQLMT]);
							
							Eeprom_WriteNBytes(ADDLIQLMT,&Cur_Param[ADDLIQLMT], 2);
            }
            break;
            
          case 0x49://漏油时间阀值，范围0-999
            if(1000 > u16Temp)
            {
              Cur_Param[SUBLIQTIME] = UART1_RXBuff[7+index];
              Cur_Param[SUBLIQTIME + 1] = UART1_RXBuff[8+index];
              ProductPara.SubLiqTime = u16Temp;
//              I2C1_WNBytesMul3T(EEPROM_ADDRESS, SUBLIQTIME, 2, &Cur_Param[SUBLIQTIME]);
							
							Eeprom_WriteNBytes(SUBLIQTIME,&Cur_Param[SUBLIQTIME], 2);
            }
            break;
            
          case 0x4A://漏油量阀值，范围1-9999
            if((0 < u16Temp)&&(10000 > u16Temp))
            {
              Cur_Param[SUBLIQLMT] = UART1_RXBuff[7+index];
              Cur_Param[SUBLIQLMT + 1] = UART1_RXBuff[8+index];
              ProductPara.SubLiqCnt = u16Temp;
//              I2C1_WNBytesMul3T(EEPROM_ADDRESS, SUBLIQLMT, 2, &Cur_Param[SUBLIQLMT]);
							
							Eeprom_WriteNBytes(SUBLIQLMT,&Cur_Param[SUBLIQLMT], 2);
            }
            break;
            
          case 0x70://车速Km
            RunVar.CarSpeed = u16Temp;
            if((0 == RunVar.CarSpeed)&&(65535 > RunVar.LowSpeedCount))
            {
              RunVar.LowSpeedCount++;
            }
            else if(0 != RunVar.CarSpeed)
            {
              RunVar.LowSpeedCount = 0;
            }
            
            if(10 <= RunVar.LowSpeedCount)
            {
              RunVar.FiltSpeed = 0;
            }
            else
            {
              RunVar.FiltSpeed = (RunVar.FiltSpeed + RunVar.CarSpeed) / 2;
            }
            break;
            
          case 0x71://ACC状态
            RunVar.AccStatus = UART1_RXBuff[8+index];
            break;
            
          case 0x72://里程高位
            RunVar.CarMileage &= 0x0000FFFF;
            RunVar.CarMileage |= (uint32_t)(u16Temp << 16);
            break;
            
          case 0x73://里程低位
            RunVar.CarMileage &= 0xFFFF0000;
            RunVar.CarMileage |= u16Temp;
            break;
            
          default:
            break;
          }
          
          index += 2;
        }
        
        ProductPara.bFlashWEn = FLASH_WR_DISABLE;
        
        for(i = 0; i < 6; i++)
        {
          SendBuf[i] = UART1_RXBuff[i];
        }
        
        MBASCII_SendMsg(UART1_RXBuff,6);
}

//**************************************************************************************************
// 名称         	: MBASCII_Fun25()
// 创建日期     	: 2016-10-20
// 作者        	 	: 庄明群
// 功能         	: Modbus 25功能处理
// 输入参数     	: 无
// 输出参数     	: 无
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************
//3a 34 31  32 35 34 31  30 30 30 30  30 30 35 39 d a
void MBASCII_Fun25(void)
{
    uint8_t ReadAdrH;
    uint8_t ReadAdrL;
    uint8_t Data_Buf[4];
    
    ReadAdrH = UART1_RXBuff[2];
    ReadAdrL = UART1_RXBuff[3];
    
    SendLen = 0;
    SendBuf[SendLen++] = (MBASCII_GetSlaveAddr(UART1_RXBuff)) ? UartComm.SlaveAddr : 0x00;
    SendBuf[SendLen++] = 0x25;

    //如果操作范围溢出

    if ((ReadAdrL > ASCII_SPECICAL_COIL_REGION1_END)
        && ((ReadAdrL < ASCII_SPECICAL_COIL_REGION2_BGEIN) || (ReadAdrL > ASCII_SPECICAL_COIL_REGION2_END))
        ||(ReadAdrH != UartComm.SlaveAddr))
    {rt_kprintf("F250X02\n");
        MBASCII_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
        return;
    }

    if (!(((UART1_RXBuff[4] == 0xFF) && (UART1_RXBuff[5] == 0x00))
            || ((UART1_RXBuff[4] == 0x00) && (UART1_RXBuff[5] == 0x00))))
    {rt_kprintf("F250X03\n");
        MBASCII_SendErr(MB_EX_ILLEGAL_DATA_VALUE);
        return;
    }

    if (ProductPara.bFlashWEn == FLASH_WR_DISABLE)
    {rt_kprintf("F250X04\n");
        MBASCII_SendErr(MB_EX_SLAVE_DEVICE_FAILURE);
        return;
    }

    switch(ReadAdrL)
    {
    case 0x00:
      if((UART1_RXBuff[4] == 0x00) && (UART1_RXBuff[5] == 0x00))//标定低液位
      { rt_kprintf("F250标定低液位\n");
        Cur_Param[CAPMIN] = (unsigned char)(RunVar.RawCap >> 24);
        Cur_Param[CAPMIN+1] = (unsigned char)(RunVar.RawCap >> 16);
        Cur_Param[CAPMIN+2] =(unsigned char)(RunVar.RawCap >> 8);
        Cur_Param[CAPMIN+3] = (unsigned char)RunVar.RawCap;
        RunVar.CalState |= CalibrateLow;
      }
      else if((UART1_RXBuff[4] == 0xFF) && (UART1_RXBuff[5] == 0x00))//标定高液位
      {rt_kprintf("F250标定高液位\n");
        Cur_Param[CAPMAX] = (unsigned char)(RunVar.RawCap >> 24);
        Cur_Param[CAPMAX+1] = (unsigned char)(RunVar.RawCap >> 16);
        Cur_Param[CAPMAX+2] =(unsigned char)(RunVar.RawCap >> 8);
        Cur_Param[CAPMAX+3] = (unsigned char)RunVar.RawCap;
        RunVar.CalState |= CalibrateHeight;
      }
      CapCalibrate();
      break;
      
    case 0x01:
      if((UART1_RXBuff[4] == 0xFF) && (UART1_RXBuff[5] == 0x00))
      {rt_kprintf("F2500x01_bDADebugEn set\n");
        DAOilDebug.bDADebugEn = Bit_SET;
        DAOutDebug.bDADebugEn = Bit_RESET;
      }
      else
      {rt_kprintf("F250bDADebugEn reset\n");
        DAOilDebug.bDADebugEn = Bit_RESET;
      }
      break;
      
    case 0x02:
      if((UART1_RXBuff[4] == 0xFF) && (UART1_RXBuff[5] == 0x00))
      {rt_kprintf("F250b0x02_DADebugEn set\n");
        DAOutDebug.bDADebugEn = Bit_SET;
        DAOilDebug.bDADebugEn = Bit_RESET;
      }
      else
      {rt_kprintf("F250b0x02_DADebugEn reset\n");
        DAOutDebug.bDADebugEn = Bit_RESET;
      }
      break;
      
    case 0x03:
      if(Bit_SET == DAOutDebug.bDADebugEn)
      {
        if((UART1_RXBuff[4] == 0x00) && (UART1_RXBuff[5] == 0x00))//零点DA 码
        {rt_kprintf("F250b0x03零点DA 码\n");
          ProductPara.DAMin = DAOutDebug.usDACode;
          floatToHexArray(ProductPara.DAMin, Data_Buf, 4);
//          I2C1_WNBytesMul3T(EEPROM_ADDRESS, DAMIN, 4, Data_Buf);
					Eeprom_WriteNBytes(DAMIN, Data_Buf, 4);
        }
        else if((UART1_RXBuff[4] == 0xFF) && (UART1_RXBuff[5] == 0x00))//满程DA 码
        {rt_kprintf("F250b0x03满程DA 码\n");
          ProductPara.DAMax = DAOutDebug.usDACode;
          floatToHexArray(ProductPara.DAMax, Data_Buf, 4);
//          I2C1_WNBytesMul3T(EEPROM_ADDRESS, DAMAX, 4, Data_Buf);
					Eeprom_WriteNBytes(DAMAX, Data_Buf, 4);
        }
      }
      break;
      
    case 0x40:
			rt_kprintf("F250x40_NVIC_SystemReset\n");
      NVIC_SystemReset();
      break;
      
    default:
      break;
    }
    
    MBASCII_SendMsg(UART1_RXBuff, 6);
}

//**************************************************************************************************
// 名称         	: MBASCII_Fun26()
// 创建日期     	: 2016-10-20
// 作者        	 	: 庄明群
// 功能         	: Modbus 26功能处理
// 输入参数     	: 无
// 输出参数     	: 无
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

void MBASCII_Fun26(void)
{
    uint8_t i;
    uint8_t ReadAdrH;
    uint8_t ReadAdrL;
    uint32_t Data_Buf;
    float fTemp = 0;
    
    ReadAdrH = UART1_RXBuff[2];
    ReadAdrL = UART1_RXBuff[3];
    
    uint16_t Register_Num = (uint16_t)UART1_RXBuff[4] * 256 + UART1_RXBuff[5];
    
    SendLen = 0;
    SendBuf[SendLen++] = (MBASCII_GetSlaveAddr(UART1_RXBuff)) ? UartComm.SlaveAddr : 0x00;
    SendBuf[SendLen++] = 0x26;
    SendBuf[SendLen++] = Register_Num * 2;      //数据长度

    if (!(((ReadAdrL <= ASCII_DB_HOLD_REG_REGION1_END) && (ReadAdrL + Register_Num) <= (ASCII_DB_HOLD_REG_REGION1_END + 2))
               || ((ReadAdrL >= ASCII_DB_HOLD_REG_REGION2_BEGIN) && (ReadAdrL <= ASCII_DB_HOLD_REG_REGION2_END) && (ReadAdrL + Register_Num) <= (ASCII_DB_HOLD_REG_REGION2_END + 2))
               || ((ReadAdrL >= ASCII_DB_HOLD_REG_REGION3_BEGIN) &&(ReadAdrL <= ASCII_DB_HOLD_REG_REGION3_END) && (ReadAdrL + Register_Num) <= (ASCII_DB_HOLD_REG_REGION3_END + 2)))
               || (0 == Register_Num) || (0 != (Register_Num & 0x01)) || (0 != (ReadAdrL & 0x01)) || (ReadAdrH != UartComm.SlaveAddr))
    {
        MBASCII_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
        return;
    }
    
    for (long k = 0; k < Register_Num; ReadAdrL += 2, k += 2)
    {
      switch(ReadAdrL)
      {
      case 0x0E:
        fTemp = ProductPara.Floater.Scale[(ReadAdrL - ASCII_DB_HOLD_REG_REGION1_BEGIN) >> 1];
        Data_Buf = *(uint32_t*) & fTemp;
        break;
        
      case 0x1E:
        fTemp = ProductPara.Floater.DACode[(ReadAdrL -(ASCII_DB_HOLD_REG_REGION1_BEGIN + 0x10)) >> 1];
        Data_Buf = *(uint32_t*) & fTemp;
        break;
        
      case 0x80:
        Data_Buf = *(uint32_t*) &  RunVar.RawCap;
        break;
        
      case 0x82:
        Data_Buf = *(uint32_t*) & ProductPara.CapMin;
        break;
        
      case 0x84:
        Data_Buf = *(uint32_t*) & ProductPara.CapMax;
        break;
        
      case 0x86:
        Data_Buf = *(uint32_t*) & ProductPara.DAMin;
        break;
        
      case 0x88:
        Data_Buf = *(uint32_t*) & ProductPara.DAMax;
        break;        
        
      case 0x90:
        Data_Buf = *(uint32_t*) & ProductPara.Temp_K;
        break;
        
      case 0x92:
        Data_Buf = *(uint32_t*) & ProductPara.Temp_B;
        break;
        
      case 0x94:
        Data_Buf = *(uint32_t*) & ProductPara.Temp2_K;
        break;
        
      case 0x96:
        Data_Buf = *(uint32_t*) & ProductPara.Temp2_B;
        break;
        
      case 0xB6:
        if(Bit_SET == DAOilDebug.bDADebugEn)
        {
          Data_Buf = *(uint32_t*) & DAOilDebug.usDACode;
        }
        else if(Bit_SET == DAOutDebug.bDADebugEn)
        {
          Data_Buf = *(uint32_t*) & DAOutDebug.usDACode;
        }
        else
        {
          Data_Buf = 0;
        }
        break;
        
      default:
        Data_Buf = 0;
        break;
      }
      
      for (i = 4; i > 0; i--)
      {
        SendBuf[SendLen++] = (uint8_t)(Data_Buf >> ((i - 1) * 8));
      }
    }

    MBASCII_SendMsg(SendBuf, SendLen);
}

//**************************************************************************************************
// 名称         	: MBASCII_Fun27()
// 创建日期     	: 2016-10-20
// 作者        	 	: 庄明群
// 功能         	: Modbus 27功能处理
// 输入参数     	: 无
// 输出参数     	: 无
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

void MBASCII_Fun27(void)
{
    uint8_t ReadAdrH;
    uint8_t ReadAdrL;
    
    uint16_t Register_Num = (uint16_t)UART1_RXBuff[4] * 256 + UART1_RXBuff[5];
    uint32_t index = 0;

    ReadAdrH = UART1_RXBuff[2];
    ReadAdrL = UART1_RXBuff[3];
    
    SendLen = 0;
    SendBuf[SendLen++] = (MBASCII_GetSlaveAddr(UART1_RXBuff)) ? UartComm.SlaveAddr : 0x00;
    SendBuf[SendLen++] = 0x27;
    SendBuf[SendLen++] = Register_Num * 2;

    if (!(((ReadAdrL <= ASCII_DB_HOLD_REG_REGION1_END) && (ReadAdrL + Register_Num) <= (ASCII_DB_HOLD_REG_REGION1_END + 2))
               || ((ReadAdrL >= ASCII_DB_HOLD_REG_REGION2_BEGIN) && (ReadAdrL <= ASCII_DB_HOLD_REG_REGION2_END) && (ReadAdrL + Register_Num) <= (ASCII_DB_HOLD_REG_REGION2_END + 2))
               || ((ReadAdrL >= ASCII_DB_HOLD_REG_REGION3_BEGIN) && (ReadAdrL <= ASCII_DB_HOLD_REG_REGION3_END) && (ReadAdrL + Register_Num) <= (ASCII_DB_HOLD_REG_REGION3_END + 2)))
             ||(0 != (Register_Num & 0x01))||(0 == Register_Num)||(0 != (ReadAdrL & 0x01))||((Register_Num * 2) != UART1_RXBuff[6])||(ReadAdrH != UartComm.SlaveAddr))
    {
        MBASCII_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
        return;
    }

    if (ProductPara.bFlashWEn == FLASH_WR_DISABLE)
    {
        MBASCII_SendErr(MB_EX_SLAVE_DEVICE_FAILURE);
        return;
    }

    for (long k = 0; k < Register_Num; ReadAdrL += 2, k += 2)
    {
      switch(ReadAdrL)
      {
      case 0x0E:
        if(DAOilDebug.bDADebugEn == Bit_SET)
        {
          ProductPara.Floater.Scale[(ReadAdrL - ASCII_DB_HOLD_REG_REGION1_BEGIN) >> 1] = (unsigned short)HexToFloat(&UART1_RXBuff[7 + index]);
          I2C1_WNBytesMul3T(EEPROM_ADDRESS, CALIB_FLOATER_SCALE_1 + 2 * ((ReadAdrL - ASCII_DB_HOLD_REG_REGION1_BEGIN) >> 1),
                                 2, (uint8_t *)&(ProductPara.Floater.Scale[(ReadAdrL - ASCII_DB_HOLD_REG_REGION1_BEGIN) >> 1]));
        }
        break;
        
      case 0x1E:
        if(DAOilDebug.bDADebugEn == Bit_SET)
        {
          ProductPara.Floater.DACode[(ReadAdrL - ASCII_DB_HOLD_REG_REGION1_BEGIN - 0x10) >> 1] = (unsigned short)HexToFloat(&UART1_RXBuff[7 + index]);
          I2C1_WNBytesMul3T(EEPROM_ADDRESS, CALIB_FLOATER_DA_1 + 2 * ((ReadAdrL - ASCII_DB_HOLD_REG_REGION1_BEGIN - 0x10) >> 1),
                                 2, (uint8_t *)&(ProductPara.Floater.DACode[(ReadAdrL -ASCII_DB_HOLD_REG_REGION1_BEGIN - 0x10) >> 1]));
        }
        break;
        
      case 0x80:
        ProductPara.Def_Range = HexToFloat(&UART1_RXBuff[7 + index]);
        I2C1_WNBytesMul3T(EEPROM_ADDRESS, DEF_RANGE, 4, &UART1_RXBuff[7 + index]);
        break;
        
      case 0x82:
        ProductPara.Def_CapMin = HexToFloat(&UART1_RXBuff[7 + index]);
        I2C1_WNBytesMul3T(EEPROM_ADDRESS, DEF_CAPMIN, 4, &UART1_RXBuff[7 + index]);
        break;
        
      case 0x84:
        ProductPara.Def_CapMax = HexToFloat(&UART1_RXBuff[7 + index]);
        I2C1_WNBytesMul3T(EEPROM_ADDRESS, DEF_CAPMAX, 4, &UART1_RXBuff[7 + index]);
        break;
        
      case 0x86:
        ProductPara.DAMin = HexToFloat(&UART1_RXBuff[7 + index]);
        I2C1_WNBytesMul3T(EEPROM_ADDRESS, DAMIN, 4, &UART1_RXBuff[7 + index]);
        break;
        
      case 0x88:
        ProductPara.DAMax = HexToFloat(&UART1_RXBuff[7 + index]);
        I2C1_WNBytesMul3T(EEPROM_ADDRESS, DAMAX, 4, &UART1_RXBuff[7 + index]);
        break;        
        
      case 0x90:
        ProductPara.Temp_K = HexToFloat(&UART1_RXBuff[7 + index]);
        I2C1_WNBytesMul3T(EEPROM_ADDRESS, TEMP_K, 4, &UART1_RXBuff[7 + index]);
        break;
        
      case 0x92:
        ProductPara.Temp_B = HexToFloat(&UART1_RXBuff[7 + index]);
        I2C1_WNBytesMul3T(EEPROM_ADDRESS, TEMP_B, 4, &UART1_RXBuff[7 + index]);
        break;
        
      case 0x94:
        ProductPara.Temp2_K = HexToFloat(&UART1_RXBuff[7 + index]);
        I2C1_WNBytesMul3T(EEPROM_ADDRESS, TEMP2_K, 4, &UART1_RXBuff[7 + index]);
        break;
        
      case 0x96:
        ProductPara.Temp2_B = HexToFloat(&UART1_RXBuff[7 + index]);
        I2C1_WNBytesMul3T(EEPROM_ADDRESS, TEMP2_B, 4, &UART1_RXBuff[7 + index]);
        break;
        
      case 0xB6:
        if(DAOilDebug.bDADebugEn == Bit_SET)
        {
          DAOilDebug.usDACode = HexToFloat(&UART1_RXBuff[7 + index]);
          DA_Write((unsigned short)DAOilDebug.usDACode);
        }
        else if(DAOutDebug.bDADebugEn == Bit_SET)
        {
          DAOutDebug.usDACode = HexToFloat(&UART1_RXBuff[7 + index]);
          DA2_Write((unsigned short)DAOutDebug.usDACode);
        }
        break;
        
      default:
        break;
      }
      
      index += 4;
    }

    MBASCII_SendMsg(UART1_RXBuff, 6);
}

//**************************************************************************************************
// 名称         	: MBASCII_Fun2A()
// 创建日期     	: 2016-10-20
// 作者        	 	: 庄明群
// 功能         	: 写版本信息
// 输入参数     	: 无
// 输出参数     	: 无
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

void MBASCII_Fun2A(void)
{
    uint8_t i,j,objlen;
    uint8_t ReadAdr = UART1_RXBuff[3];
    uint8_t Register_Num = UART1_RXBuff[5];
    SendLen = 0;

    SendBuf[SendLen++] = (MBASCII_GetSlaveAddr(UART1_RXBuff)) ? UartComm.SlaveAddr : 0x00;
    SendBuf[SendLen++] = 0x2A;
    SendBuf[SendLen++] = UART1_RXBuff[2];
    if(!((UART1_RXBuff[2] == UartComm.SlaveAddr)
       &&(ReadAdr >= ASCII_PRODUCTION_INF_BEGIN)
       &&(ReadAdr <= ASCII_PRODUCTION_INF_END)
       &&((ReadAdr + Register_Num) <= (ASCII_PRODUCTION_INF_END + 1))
       &&(0 != Register_Num)&&(0x00 == UART1_RXBuff[4])))
    {
      MBASCII_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
      return;
    }
    j = 6;
    ProductPara.bFlashWEn = FLASH_WR_ENABLE;
    for(i = 0; i < Register_Num; i++)
    {
      switch(ReadAdr)
      {
      case 0xE0://机构名称
        objlen = UART1_RXBuff[j] + 1;
        I2C1_WriteNBytes(EEPROM_ADDRESS, COMPANY, objlen, &UART1_RXBuff[j]);                                    
        j += objlen;
        ReadAdr++;
        break;
        
      case 0xE1://产品代号
        objlen = UART1_RXBuff[j] + 1;
        I2C1_WriteNBytes(EEPROM_ADDRESS, DEV_ENCODING, objlen, &UART1_RXBuff[j]);                               
        j += objlen;
        ReadAdr++;
        break;
        
      case 0xE2://硬件版本
        objlen = UART1_RXBuff[j] + 1;
        I2C1_WriteNBytes(EEPROM_ADDRESS, SENSOR_HV, objlen, &UART1_RXBuff[j]);                                  
        j += objlen;
        ReadAdr++;
        break;
        
      case 0xE3://软件版本
        objlen = UART1_RXBuff[j] + 1;
        //I2C1_WriteNBytes(EEPROM_ADDRESS, SENSOR_SV, objlen, &UART1_RXBuff[j]);                                  
        j += objlen;
        ReadAdr++;
        break;
        
      case 0xE4://设备ID
        objlen = UART1_RXBuff[j] + 1;
        I2C1_WriteNBytes(EEPROM_ADDRESS, DEV_ID, objlen, &UART1_RXBuff[j]);                                     
        j += objlen;
        ReadAdr++;
        break;
        
      case 0xE5://客户编码
        objlen = UART1_RXBuff[j] + 1;
        I2C1_WriteNBytes(EEPROM_ADDRESS, CUSTOMERCODE, objlen, &UART1_RXBuff[j]);                               
        j += objlen;
        ReadAdr++;
        break;
        
      default:
        break;
      }
    }
    ProductPara.bFlashWEn = FLASH_WR_DISABLE;
    
    MBASCII_SendMsg(UART1_RXBuff, 6);
}

//**************************************************************************************************
// 名称         	: MBASCII_Fun2B()
// 创建日期     	: 2016-10-20
// 作者        	 	: 庄明群
// 功能         	: 读版本信息
// 输入参数     	: 无
// 输出参数     	: 无
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

void MBASCII_Fun2B(void)
{
  uint8_t i,objlen;
  uint8_t ReadAdr = UART1_RXBuff[3];
  uint8_t Register_Num = UART1_RXBuff[5];
  SendLen = 0;

  SendBuf[SendLen++] = (MBASCII_GetSlaveAddr(UART1_RXBuff)) ? UartComm.SlaveAddr : 0x00;
  SendBuf[SendLen++] = 0x2B;
  SendBuf[SendLen++] = UART1_RXBuff[4];
  SendBuf[SendLen++] = UART1_RXBuff[5];
  if(!((UART1_RXBuff[2] == UartComm.SlaveAddr)
       &&(ReadAdr >= ASCII_PRODUCTION_INF_BEGIN)
       &&(ReadAdr <= ASCII_PRODUCTION_INF_END)
       &&((ReadAdr + Register_Num) <= (ASCII_PRODUCTION_INF_END + 1))
       &&(0 != Register_Num)&&(0x00 == UART1_RXBuff[4])))
  {
    MBASCII_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
    return;
  }
  for(i = 0; i < Register_Num; i++)
  {
    switch(ReadAdr)
    {
    case 0xE0://机构名称
      SendBuf[SendLen++] = 0xE0;
      I2C1_ReadNBytes(EEPROM_ADDRESS, COMPANY, 1, &objlen);                                                       
      SendBuf[SendLen++] = objlen;
      if((objlen > (DATAPACKET_LEN/3))||(0 == objlen))
      {
        SendBuf[SendLen - 1] = 1;
        SendBuf[SendLen++] = 0;
        break;
      }
      I2C1_ReadNBytes(EEPROM_ADDRESS, COMPANY + 1, objlen, &SendBuf[SendLen]);
      SendLen += objlen;
      //ReadAdr++;
      break;
      
    case 0xE1://产品代号
      SendBuf[SendLen++] = 0xE1;
      I2C1_ReadNBytes(EEPROM_ADDRESS, DEV_ENCODING, 1, &objlen);                                                  
      SendBuf[SendLen++] = objlen;
      if((objlen > (DATAPACKET_LEN/3))||(0 == objlen))
      {
        SendBuf[SendLen - 1] = 1;
        SendBuf[SendLen++] = 0;
        break;
      }
      I2C1_ReadNBytes(EEPROM_ADDRESS, DEV_ENCODING + 1, objlen, &SendBuf[SendLen]);
      SendLen += objlen;
      //ReadAdr++;
      break;
      
    case 0xE2://硬件版本
      SendBuf[SendLen++] = 0xE2;
      I2C1_ReadNBytes(EEPROM_ADDRESS, SENSOR_HV, 1, &objlen);                                                     
      SendBuf[SendLen++] = objlen;
      if((objlen > (DATAPACKET_LEN/3))||(0 == objlen))
      {
        SendBuf[SendLen - 1] = 1;
        SendBuf[SendLen++] = 0;
        break;
      }
      I2C1_ReadNBytes(EEPROM_ADDRESS, SENSOR_HV + 1, objlen, &SendBuf[SendLen]);
      SendLen += objlen;
      //ReadAdr++;
      break;
    
    case 0xE3://软件版本
      SendBuf[SendLen++] = 0xE3;
      
      SendBuf[SendLen++] = 0x07;
      
      for(objlen = 0; objlen < 7; objlen++)
      {
        SendBuf[SendLen++] = SensorSoftVersion[objlen + 1];
      }
      
      //SendLen += objlen;
      
      //ReadAdr++;
      break;
      
    case 0xE4:
      SendBuf[SendLen++] = 0xE4;
      I2C1_ReadNBytes(EEPROM_ADDRESS, DEV_ID, 1, &objlen);                                                        //设备ID
      SendBuf[SendLen++] = objlen;
      if((objlen > (DATAPACKET_LEN/3))||(0 == objlen))
      {
        SendBuf[SendLen - 1] = 1;
        SendBuf[SendLen++] = 0;
        break;
      }
      I2C1_ReadNBytes(EEPROM_ADDRESS, DEV_ID + 1, objlen, &SendBuf[SendLen]);
      SendLen += objlen;
      //ReadAdr++;
      break;
      
    case 0xE5:
      SendBuf[SendLen++] = 0xE5;
      I2C1_ReadNBytes(EEPROM_ADDRESS, CUSTOMERCODE, 1, &objlen);                                                  //客户编码
      SendBuf[SendLen++] = objlen;
      if((objlen > (DATAPACKET_LEN/3))||(0 == objlen))
      {
        SendBuf[SendLen - 1] = 1;
        SendBuf[SendLen++] = 0;
        break;
      }
      I2C1_ReadNBytes(EEPROM_ADDRESS, CUSTOMERCODE + 1, objlen, &SendBuf[SendLen]);
      SendLen += objlen;
      //ReadAdr++;
      break;
      
    default:
      break;
    }
    
    ReadAdr++;
  }
  
  MBASCII_SendMsg(SendBuf, SendLen);
}

//**************************************************************************************************
// 名称         	: MBASCII_Fun41()
// 创建日期     	: 2016-10-20
// 作者        	 	: 庄明群
// 功能         	: 在线升级功能
// 输入参数     	: 无
// 输出参数     	: 无
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

void MBASCII_Fun41(void)
{
  uint16_t ReadAdr = (uint16_t)UART1_RXBuff[2] * 256 + UART1_RXBuff[3];
  uint16_t DataLen = (uint16_t)UART1_RXBuff[4] * 256 + UART1_RXBuff[5];
  
  SendLen = 0;
  SendBuf[SendLen++] = (MBASCII_GetSlaveAddr(UART1_RXBuff)) ? UartComm.SlaveAddr : 0x00;
  SendBuf[SendLen++] = 0x41;
  SendBuf[SendLen++] = DataLen;
  
  if(0x0001 != ReadAdr)
  {
    MBASCII_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
    return;
  }
  UART1_RXBuff[5] = 0x01;
  
  if(0x0000 != DataLen)
  {
    UART1_RXBuff[6] = 0x01;
    MBASCII_SendMsg(UART1_RXBuff, 7);
    return;
  }
  
  UART1_RXBuff[6] = 0x00;
  MBASCII_SendMsg(UART1_RXBuff, 7);
  Onlineflg = 0x0C;
  ProductPara.bFlashWEn = FLASH_WR_ENABLE;
  I2C1_WriteByte(EEPROM_ADDRESS, ONLINEUPGRADE, 0x0C);
  I2C1_WNBytesMul3T(EEPROM_ADDRESS, SLAVE_ADDR, 1, &UartComm.SlaveAddr);
  ProductPara.bFlashWEn = FLASH_WR_DISABLE;
  
  if(0x0C == Onlineflg)
  { 
    rt_thread_mdelay(20);
    NVIC_SystemReset();
  }
}
//**************************************************************************************************
// 名称         	: MBASCII_Function()
// 创建日期   	        : 2016-09-01
// 作者         	: 庄明群
// 功能         	: ModbusASCII码解析
// 输入参数     	: 无
// 输出参数     	: 无
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

void MBASCII_Function(void)
{
    uint8_t RecvLen = 0;
    if(2 == MODBUS_ASCII_RecvData(UART1_RXBuff, &RecvLen))//校验错误
    { 
			rt_kprintf("cali_fail %d\n",RecvLen);
      return;
    }
    if(RecvLen <= 0)
    {
    
		rt_kprintf("cali_fail2 %d\n",RecvLen);
      return;
    }
    else if((UartComm.SlaveAddr != MBASCII_GetSlaveAddr(UART1_RXBuff))&&
            (MB_ADDRESS_BROADCAST != MBASCII_GetSlaveAddr(UART1_RXBuff)))
    {
    
		rt_kprintf("SlaveAddr_error %x %x\n",UartComm.SlaveAddr,UART1_RXBuff[0]);
      return;
    }
    else
    {
			rt_kprintf("MBASCII_GetFunCode %x %x\n",UartComm.SlaveAddr,UART1_RXBuff[1]);
        switch (MBASCII_GetFunCode(UART1_RXBuff))
        {
        case MB_FUNC_READ_HOLDING_REGISTER:	//读单个寄存器0x03
          MBASCII_Fun03();
          break;
            
        case MB_FUNC_READ_INPUT_REGISTER:		//读两个寄存器0x04
          MBASCII_Fun04();
          break;
            
        case MB_FUNC_WRITE_SINGLE_COIL:		//写单个线圈
          MBASCII_Fun05();
          break;
            
        case MB_FUNC_WRITE_MULTIPLE_REGISTERS://写多个寄存器0x10
          if((RecvLen - 7) != UART1_RXBuff[6])
          {
            return ;
          }
          MBASCII_Fun10();
          break;
          
        case 0x25:
          MBASCII_Fun25();
          break;
          
        case 0x26:
          MBASCII_Fun26();
          break;
          
        case 0x27:
          MBASCII_Fun27();
          break;
          
        case 0x2A:
          MBASCII_Fun2A();
          break;
          
        case 0x2B:
          MBASCII_Fun2B();
          break;
          
        case 0x41:
          MBASCII_Fun41();
          break;
          
	default:
          SendLen = 0;
          SendBuf[SendLen++] = MBASCII_GetSlaveAddr(UART1_RXBuff);
          SendBuf[SendLen++] = 0x80 | (uint8_t)MBASCII_GetFunCode(UART1_RXBuff);
          SendBuf[SendLen++] = MB_EX_ILLEGAL_FUNCTION;
          MBASCII_SendMsg(SendBuf, SendLen);
	  break;
        }
    } 
		
}



//**************************************************************************************************
// 名称         	: Get_TankOil()
// 创建日期   	        : 2017-10-26
// 作者         	: 庄明群
// 修改人               :
// 功能         	: 得到油量(根据标定的液位高度对应的油量计算油量)
// 输入参数     	: 无
// 输出参数     	: 无
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

uint32_t Get_TankOil(uint32_t cyLiquidHeightAD)
{
  //u32 dwTable_B;
  uint16_t countK;
  uint16_t i;
  
  for(i = 0; i < 50; i++)
  {
    countK = (i << 3);
    dwEepromHeightADValue = CALIB_Table[countK++];
    dwEepromHeightADValue = (dwEepromHeightADValue << 8) + CALIB_Table[countK++];
    dwEepromHeightADValue = (dwEepromHeightADValue << 8) + CALIB_Table[countK++];
    dwEepromHeightADValue = (dwEepromHeightADValue << 8) + CALIB_Table[countK++];

    dwEepromOilValue = CALIB_Table[countK++];
    dwEepromOilValue = (dwEepromOilValue << 8) + CALIB_Table[countK++];
    dwEepromOilValue = (dwEepromOilValue << 8) + CALIB_Table[countK++];
    dwEepromOilValue = (dwEepromOilValue << 8) + CALIB_Table[countK++];
    
    if(0 == i)
    {
      dwUpEepromHeightADValue = dwEepromHeightADValue;
      dwUpEepromOilValue = dwEepromOilValue;
    }
    
    if(cyLiquidHeightAD <= dwEepromHeightADValue)
    {
      if(0x07FFFFFF > dwEepromHeightADValue)
      {
        if(0 == i)
        {
          dwTankOilValue = dwEepromOilValue;
        }
        else
        {
          dwTable_k = (float)(dwEepromOilValue - dwUpEepromOilValue)/(float)(dwEepromHeightADValue - dwUpEepromHeightADValue);
          dwTankOilValue = (uint32_t)((cyLiquidHeightAD - dwUpEepromHeightADValue) * dwTable_k + dwUpEepromOilValue);
        }
      }
      else
      {
        dwTankOilValue = dwUpEepromOilValue;
      }
      break;
    }
    else
    {
      if(49 == i)
      {
        dwTankOilValue = dwEepromOilValue;
        break;
      }
      dwUpEepromHeightADValue = dwEepromHeightADValue;
      dwUpEepromOilValue = dwEepromOilValue;
    }
  }
  return dwTankOilValue;
}

//**************************************************************************************************
// 名称         	: MBASCII_AutoUpLoadFrame()
// 创建日期   	        : 2016-09-01
// 作者         	: 庄明群
// 功能         	: 自动上传
// 输入参数     	: 无
// 输出参数     	: 无
// 返回结果     	: 无
// 注意和说明   	:
// 修改内容     	:
//**************************************************************************************************

void MBASCII_AutoUpLoadFrame(void)
{
	uint8_t i;
	uint32_t pTemp;
        
        SendBuf[0] = UartComm.SlaveAddr;
        SendBuf[1] = MB_FUNC_READ_INPUT_REGISTER;
        SendBuf[2] = 32;

        pTemp = (uint32_t)RunVar.LiquidAD;
       
	for (i = 4; i > 0; i--)
    	{
          SendBuf[7 - i] = (uint8_t)(pTemp >> ((i - 1) * 8));
    	}
        
        pTemp = (uint32_t)((RunVar.TempInSensor + 273.15) * 10);                                           //液体温度
        for(i = 4; i > 0; i--)
        {
          SendBuf[11 - i] = (uint8_t)(pTemp >> ((i - 1) * 8));
        }
        
        pTemp = (uint32_t)((RunVar.TempInAir + 273.15) * 10);                                              //环境温度
        for(i = 4; i > 0; i--)
        {
          SendBuf[15 - i] = (uint8_t)(pTemp >> ((i - 1) * 8));
        }
        

         pTemp = ProductPara.AddOil;   
        for(i = 4; i > 0; i--)
        {
          SendBuf[19 - i] = (uint8_t)(pTemp >> ((i - 1) * 8));                                             //加油量
        }
        

        pTemp = ProductPara.SubOil;  
        for(i = 4; i > 0; i--)
        {
          SendBuf[23 - i] = (uint8_t)(pTemp >> ((i - 1) * 8));                                             //漏油量
        }
        
        pTemp = RunVar.OilQuantity; 
	for (i = 4; i > 0; i--)
        {
          SendBuf[27 - i] = (uint8_t)(pTemp >> ((i - 1) * 8));
        }
        
        
        pTemp = RunVar.LiquidPercent;                                             //液位百分比
        for (i = 4; i > 0; i--)
        {
          SendBuf[31 - i] = (uint8_t)(pTemp >> ((i - 1) * 8));
        }
        
        SendLen = 35;
        pTemp = (uint32_t)RunVar.LiquidHeight;                                                        //液位高度      
        
        for (i = 4; i > 0; i--)
        {
          SendBuf[35 - i] = (uint8_t)(pTemp >> ((i - 1) * 8));
        }
        
	MBASCII_SendMsg_NoLimit(SendBuf, SendLen);
}




void  APP_CAP_Task(void *p_arg)
{
	uint32_t UartRunCnt = 0;
	uint32_t UartRunCntPre = 0;
//	rt_err_t result = RT_NULL;
//	rt_uint32_t e;

	while (1)
	{
	//	result = rt_sem_take(cap_sem,RT_WAITING_FOREVER);
//		if(result != RT_EOK)
//		{
//			return;
//		}

//		if (rt_event_recv(&cap_event,
//												1,
//												RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
//												RT_WAITING_FOREVER,
//												&e) == RT_EOK)
  if(1)
			{ 	
			  rt_kprintf("APP_CAP_Task\n");
				MBASCII_Function();
				rt_thread_mdelay(1000);
			}
			else									
			{
				UartRunCnt += UART_WAIT_TIME;
				if ((ProductPara.AutoUpLoadTime >= 0x02)&&(ProductPara.AutoUpLoadTime <= 0x04))
				{
					if ((UartRunCnt - UartRunCntPre) >= ((uint32_t)(ProductPara.AutoUpLoadTime -1) * 10000))
					{rt_kprintf("AutoUpLoad_comin\n");
					  MBASCII_AutoUpLoadFrame();
					  UartRunCntPre = UartRunCnt;
					}
				}
				else
				{
				  UartRunCntPre = UartRunCnt;
				}
		}
		rt_thread_mdelay(1000);
	}
}
