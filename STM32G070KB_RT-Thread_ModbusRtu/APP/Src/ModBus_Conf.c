/**@file        Modbus_Conf.c
* @brief        Modbus 功能码处理配置
* @details      对上位机发送的指令进行解析并响应,所有功能码和自动上传的程序代码都编写在本文件
* @author       杨春林
* @date         2020-04-29
* @version      V1.0.0
* @copyright    2020-2030,深圳市信为科技发展有限公司
**********************************************************************************
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/04/29  <td>1.0.0    <td>杨春林    <td>创建初始版本
* </table>
*
**********************************************************************************
*/

#include "ModBusRtu.h"


#ifdef __PICOCAP_APP_H

static uint32_t Calib_CapMin;                   ///< 标定电容零点值
static uint32_t Calib_CapMax;                   ///< 标定电容满点值
static uint16_t Calib_CapDAMin;                 ///< 标定电容DA零点值
static uint16_t Calib_CapDALow;                 ///< 标定电容DA下刻度
static uint16_t Calib_CapDAHigh;                ///< 标定电容DA上刻度
static uint16_t Calib_CapDAMax;                 ///< 标定电容DA满点值
static uint16_t Calib_TempDAMin;                ///< 标定温度DA零点值
static uint16_t Calib_TempDAMax;                ///< 标定温度DA满量程
static uint16_t Calib_ADMin;                    ///< 标定电容AD零点值
static uint16_t Calib_ADLow;                    ///< 标定电容AD下刻度
static uint16_t Calib_ADHigh;                   ///< 标定电容AD上刻度
static uint16_t Calib_ADMax;                    ///< 标定电容AD满量程
static uint8_t CapDA_ClibFlag = CALIB_CLEAR;    

//Modbus 串口重新初始化回调函数
static int MB_USART_ReInit(ModBusBaseParam_TypeDef *ModBusBaseParam);

//Modbus 设备冻结回调函数
static int MB_Freeze(ModBusBaseParam_TypeDef *ModBusBaseParam);

//系统复位回调函数
static int MB_System_Reset(ModBusBaseParam_TypeDef *ModBusBaseParam);

//无地址检查发送数据
static int MB_SendData_NoCheck(ModBusBaseParam_TypeDef *ModBusBaseParam);


/**@brief       Modbus 读预处理,从接收的原始数据中提取出消息ID和寄存器数量,将设备地址和功能码存入发送缓存中
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;
* @param[out]   ReadAddr : 读取地址(消息ID)
* @param[out]   RegNum : 寄存器数量
* @return       函数执行结果
* - None
* @note         寄存器地址不能越界
*/
static void ModBus_ReadPreHandle(ModBusBaseParam_TypeDef *ModBusBaseParam, uint16_t *ReadAddr, uint16_t *RegNum)
{
    if(ReadAddr != NULL)
    {
        //寄存器地址高字节
        *ReadAddr = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[2];
        *ReadAddr <<= 8;
        //寄存器地址高字节
        *ReadAddr |= ModBusBaseParam->ModBus_TX_RX.Receive_Buf[3];
    }

    if(RegNum != NULL)
    {
        //寄存器数据高字节
        *RegNum = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[4];
        *RegNum <<= 8;
        //寄存器数量低字节
        *RegNum |= ModBusBaseParam->ModBus_TX_RX.Receive_Buf[5];
    }
    
    //拷贝设备地址、功能码
    memcpy( ModBusBaseParam->ModBus_TX_RX.Send_Buf, 
            ModBusBaseParam->ModBus_TX_RX.Receive_Buf,
            2);
    ModBusBaseParam->ModBus_TX_RX.Send_Len = 2;
}

/**@brief       Modbus 写预处理,从接收的原始数据中提取出消息ID,将设备地址、
* 功能码、消息ID和消息长度存入发送缓存中
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;
* @param[out]   WriteAddr : 写入地址(消息ID)
* @param[out]   RegNum : 寄存器数量
* @return       函数执行结果
* - None
* @note         寄存器地址不能越界
*/
static void ModBus_WritePreHandle(ModBusBaseParam_TypeDef *ModBusBaseParam, uint16_t *WriteAddr, uint16_t *RegNum)
{
    if(WriteAddr != NULL)
    {
        //寄存器地址高字节
        *WriteAddr = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[2];
        *WriteAddr <<= 8;
        //寄存器地址高字节
        *WriteAddr |= ModBusBaseParam->ModBus_TX_RX.Receive_Buf[3];
    }

    if(RegNum != NULL)
    {
        //寄存器数据高字节
        *RegNum = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[4];
        *RegNum <<= 8;
        //寄存器数量低字节
        *RegNum |= ModBusBaseParam->ModBus_TX_RX.Receive_Buf[5];
    }

    //将响应数据存入发送缓存
    memcpy( ModBusBaseParam->ModBus_TX_RX.Send_Buf, 
            ModBusBaseParam->ModBus_TX_RX.Receive_Buf,
            6);

    ModBusBaseParam->ModBus_TX_RX.Send_Len = 6;
}

/**@brief       Modbus 03功能码消息帧处理
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;
* @param[in]    arg : 用户自定义的参数,这里为设备参数
* @return       函数执行结果
* - None
*/
void ModbusFunc03(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg)
{
    uint16_t Nr;
    //寄存器数据
    uint16_t DataBuf;
    //寄存器地址
    uint16_t ReadAddr;
    //寄存器数量
    uint16_t RegNum;
    //设备参数
    ModBus_Device_Param *Device_Param;
        
    Device_Param = (ModBus_Device_Param *)arg;
    //读预处理
    ModBus_ReadPreHandle(ModBusBaseParam, &ReadAddr, &RegNum);
    //访问地址不在有效范围内
    if( ReadAddr < HOLDING_REG_REGION1_BGEIN 
        || (ReadAddr + RegNum) > (HOLDING_REG_REGION1_END + 1)
        || RegNum == 0)
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
        //访问地址无效
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_ADDR_EXCEPTION;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
        return;
    }

    ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++]
        = RegNum * 2;
    for(Nr = 0; Nr < RegNum; Nr++, ReadAddr++)
    {
        switch(ReadAddr)
        {
        case 0x0030:
            //设备地址
            DataBuf = ModBusBaseParam->Device_Addr;
            if(BROADCASTADDR == ModBusBaseParam->ModBus_TX_RX.Receive_Buf[0])
            {
                ModBusBaseParam->ModBus_CallBack = MB_SendData_NoCheck;
            }
        break;
                
        case 0x0031:
            //波特率
            DataBuf = ModBusBaseParam->BaudRate;
        break;
                
        case 0x0032:
            //奇偶校验
            DataBuf = ModBusBaseParam->Parity;
        break;

        case 0x0033:
            //空
            DataBuf = 0;
        break;

        case 0x0034:
            //补偿使能
            DataBuf = Device_Param->PCap_DataConvert->CompenEn;
        break;

        case 0x0035:
            //滤波系数
            DataBuf = Device_Param->DataFilter->FilterFactor;
        break;

        case 0x0036:
            //自动上传周期
            DataBuf = ModBusBaseParam->AutoUpload;
        break;

        case 0x0037:
            //电容修正系数K
            DataBuf = (uint16_t)(Device_Param->PCap_DataConvert->Correct_K * 100);
        break;

        case 0x0038:
            //电容修正系数B
            DataBuf = (uint16_t)Device_Param->PCap_DataConvert->Correct_B + 100;
        break;

        case 0x0039:
            //输出方式 保留
            DataBuf = ModBusBaseParam->Output_Mode;
        break;

        case 0x003F:
            //量程 保留
            DataBuf = Device_Param->PCap_DataConvert->HeightRange;
        break;

        case 0x0040:
            //空
            DataBuf = 0;
        break;

        case 0x0041:
            //空
            DataBuf = 0;
        break;

        default:
            DataBuf = 0;
        break;
        }
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] 
            = (uint8_t)(DataBuf >> 8);
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] 
            = (uint8_t)DataBuf;       
    }
}


/**@brief       Modbus 04功能码消息帧处理
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;
* @param[in]    arg : 用户自定义的参数,这里为设备参数
* @return       函数执行结果
* - None
*/
void ModbusFunc04(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg)
{
    //浮点数据
    float fbuf;
    uint16_t Nr;
    //寄存器地址
    uint16_t ReadAddr;
    //寄存器数量
    uint16_t RegNum;
    //寄存器数据
    uint32_t DataBuf;
	
    //设备参数
    ModBus_Device_Param *Device_Param;
    extern ModBus_Device_Param         ModBus_Device;  	//ModBus管理设备的参数结构体
    
    Device_Param = (ModBus_Device_Param *)arg;
    //读预处理
    ModBus_ReadPreHandle(ModBusBaseParam, &ReadAddr, &RegNum);
    //寄存器地址无效
    if( (ReadAddr >> 8) != ModBusBaseParam->Device_Addr ||
		((ReadAddr & 0xFF) + RegNum) > (INPUT_REG_REGION2_END + 2)
        || (ReadAddr & 0x01) != 0 || (RegNum & 0x01) != 0
        || RegNum == 0)
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_ADDR_EXCEPTION;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
        return;
    }

    ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++]
        = RegNum * 2;
    for(Nr = 0; Nr < RegNum; Nr += 2, ReadAddr += 2)
    {
        switch(ReadAddr & 0xFF)
        {
        case 0x0000:
            //液位高度AD值
//            fbuf = (float)Device_Param->PCap_DataConvert_Out->LiquidHeightAD;
            DataBuf =  (ModBus_Device.temperature+2731);
        break;
            
        case 0x0002:
            //环境温度
            DataBuf = *(uint32_t *)&Device_Param->ADC_TemperOut->TemperInAir;
        break;
            
        case 0x0004:
            //液体温度 保留
            DataBuf = *(uint32_t *)&Device_Param->ADC_TemperOut->TemperInLiquid;
        break;

        case 0x0080:
            //PCap原始值
            fbuf = (float)Device_Param->PCap_DataConvert_Out->PCap_ResultValue;
            DataBuf = *(uint32_t *)&fbuf;
        break;

        default:
            DataBuf = 0;
        break;
        }

        ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] 
            = (DataBuf >> 24);
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] 
            = (DataBuf >> 16);
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] 
            = (DataBuf >> 8);
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] 
            = (uint8_t)DataBuf;        
    }
}

/**@brief       Modbus 05功能码消息帧处理
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;
* @param[in]    arg : 用户自定义的参数,这里为设备参数
* @return       函数执行结果
* - None
*/
void ModbusFunc05(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg)
{
    //寄存器地址
    uint16_t WriteAddr;
    //寄存器数据
    uint16_t DataBuf;
    static uint8_t CalibFlag = CALIB_CLEAR;
    //设备参数
    ModBus_Device_Param *Device_Param;
        
    Device_Param = (ModBus_Device_Param *)arg;
    //写预处理
    ModBus_WritePreHandle(ModBusBaseParam, &WriteAddr, NULL);    
    //地址无效
    if((WriteAddr < SINGLE_COIL_REGION1_BEGIN || WriteAddr > SINGLE_COIL_REGION1_END)
        && WriteAddr != 0x004A)
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_ADDR_EXCEPTION;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
        return;
    }

    //数据内容高字节
    DataBuf = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[4];
    DataBuf <<= 8;
    //数据内容低字节
    DataBuf |= ModBusBaseParam->ModBus_TX_RX.Receive_Buf[5];

    //数据内容无效
    if((0x0000 != DataBuf)&&(0xFF00 != DataBuf))
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_VALU_EXCEPTION;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
        return;
    }
     
    switch(WriteAddr)
    {
    case 0x004A:                                //为了跟老协议兼容
        //内部存储器使能或禁止
        if(0xFF00 == DataBuf)
        {
            ModBusBaseParam->InRomWrEn = IN_FLASH_WR_ENABLE;
        }
        else if(0x0000 == DataBuf)
        {
            ModBusBaseParam->InRomWrEn = IN_FLASH_WR_DISABLE;
        }
    break;      
      
    case 0x0050:
        if(ModBusBaseParam->InRomWrEn == IN_FLASH_WR_ENABLE)
        {
            //电容标定
            if(0xFF00 == DataBuf)       //标定满量程
            {
                Calib_CapMax = Device_Param->PCap_DataConvert_Out->PCap_ResultValue;
                CalibFlag |= CALIB_CAPMAX_FLAG;
            }
            else if(0x0000 == DataBuf)  //标定零点
            {
                Calib_CapMin = Device_Param->PCap_DataConvert_Out->PCap_ResultValue;
                CalibFlag |= CALIB_CAPMIN_FLAG;
            }
            //电容标定标志位有效就写入标定内容  
            if(CALIB_CAPEOC_FLAG == CalibFlag)
            {
                MB_Cap_Calibration(arg);
                CalibFlag = CALIB_CLEAR;
            }
        }
        else
        {
            goto __rom_error;
        }
    break;

    case 0x0051:
        if(ModBusBaseParam->InRomWrEn == IN_FLASH_WR_ENABLE)
        {
            //恢复电容标定出厂值
            if(0xFF00 == DataBuf)
            {
                Calib_CapMin = Device_Param->PCap_DataConvert->CapMinBak;
                Calib_CapMax = Device_Param->PCap_DataConvert->CapMaxBak;
                MB_Cap_Calibration(arg);
            }
        }
        else
        {
            goto __rom_error;
        }
    break;

    case 0x0052:
        //设备冻结或解冻
        if(0xFF00 == DataBuf)
        {
            ModBusBaseParam->ModBus_CallBack = MB_Freeze;
        }
        else if(0x0000 == DataBuf)
        {
            ModBusBaseParam->Freeze = FREEZE_DISABLE;
        }
    break;

    case 0x0053:
        //内部Flash使能或禁止
        if(0xFF00 == DataBuf)
        {
            ModBusBaseParam->InRomWrEn = IN_FLASH_WR_ENABLE;
        }
        else if(0x0000 == DataBuf)
        {
            ModBusBaseParam->InRomWrEn = IN_FLASH_WR_DISABLE;
        }
    break;
            
    default:
    break;
    }	
    return;
__rom_error:
    ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
    ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_DEVC_EXCEPTION;
    ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;    
}

/**@brief       Modbus 10功能码消息帧处理
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;
* @param[in]    arg : 用户自定义的参数,这里为设备参数
* @return       函数执行结果
* - None
*/
void ModbusFunc10(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg)
{
    uint16_t Nr;
    //内容索引
    uint16_t Index;
    //寄存器地址
    uint16_t WriteAddr;
    //寄存器数量
    uint16_t RegNum;
    //数据长度
    uint16_t DataLen;
    //16位数据暂存
    uint16_t u16temp;
    //设备参数
    ModBus_Device_Param *Device_Param;
        
    Device_Param = (ModBus_Device_Param *)arg;
    
    //数据长度
    DataLen = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[6];
    //写预处理
    ModBus_WritePreHandle(ModBusBaseParam, &WriteAddr, &RegNum);    
    //寄存器地址无效
    if( WriteAddr < MUL_REG_REGION1_BEGIN 
        || (WriteAddr + RegNum) > (MUL_REG_REGION1_END + 1)
        || (DataLen != (RegNum * 2))
        || RegNum == 0)
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_ADDR_EXCEPTION;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
        return;
    }

    //内部ROM访问禁止
    if(IN_FLASH_WR_ENABLE != ModBusBaseParam->InRomWrEn)
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_DEVC_EXCEPTION;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
        return;
    }

    Index = 0;
    for(Nr = 0; Nr < RegNum; Nr++, WriteAddr++)
    {
        u16temp = (uint16_t)(ModBusBaseParam->ModBus_TX_RX.Receive_Buf[7 + Index] << 8) 
            | ModBusBaseParam->ModBus_TX_RX.Receive_Buf[8 + Index];

        switch(WriteAddr)
        {
        case 0x0030:
            //设备地址
            if(0 < u16temp && 0xF8 > u16temp)
            {
                ModBusBaseParam->Device_Addr = u16temp;
                InFlash_Write3T_MultiBytes(DEVICE_ADDR, &ModBusBaseParam->Device_Addr, 1);
            }
        break;
                
        case 0x0031:
            //波特率
            if(0x06 > u16temp)
            {
                ModBusBaseParam->BaudRate = u16temp;
                InFlash_Write3T_MultiBytes(BAUDRATE, &ModBusBaseParam->BaudRate, 1);
                ModBusBaseParam->ModBus_CallBack = MB_USART_ReInit;
            }
        break;
                
        case 0x0032:
            //奇偶校验
            if(0x03 > u16temp)
            {
                ModBusBaseParam->Parity = u16temp;
                InFlash_Write3T_MultiBytes(PARITY, &ModBusBaseParam->Parity, 1);
                ModBusBaseParam->ModBus_CallBack = MB_USART_ReInit;
            }
        break;
          
        case 0x0033:
            //空
        break;
          
        case 0x0034:
            //补偿使能
            if(2 > u16temp)
            {
                Device_Param->PCap_DataConvert->CompenEn = u16temp;
                InFlash_Write3T_MultiBytes(COMPENSATE, &Device_Param->PCap_DataConvert->CompenEn, 1);
            }
        break;
          
        case 0x0035:
            //滤波系数
            if(0x0A > u16temp)
            {
                Device_Param->DataFilter->FilterFactor = u16temp;
                InFlash_Write3T_MultiBytes(FILTER, &Device_Param->DataFilter->FilterFactor, 1);
                SwitchCurFilter(Device_Param->DataFilter->FilterFactor, Device_Param->DataFilter);
            }
        break;
          
        case 0x0036:
            //自动上传周期
            ModBusBaseParam->AutoUpload = u16temp;
            InFlash_Write3T_MultiBytes(AUTO_UPLOAD, &ModBusBaseParam->AutoUpload, 1);
        break;
          
        case 0x0037:
            //电容修正系数K
            if(0 < u16temp)
            {
                Device_Param->PCap_DataConvert->Correct_K = (float)u16temp / 100.0;
                InFlash_Write3T_MultiBytes(CORRECT_K, 
                                    (const uint8_t *)&ModBusBaseParam->ModBus_TX_RX.Receive_Buf[7 + Index], 2);
            }
        break;
          
        case 0x0038:
            //电容修正系数B
            Device_Param->PCap_DataConvert->Correct_B = (float)u16temp;
            InFlash_Write3T_MultiBytes(CORRECT_B, 
                                    (const uint8_t *)&ModBusBaseParam->ModBus_TX_RX.Receive_Buf[7 + Index], 2);
        break;
          
        case 0x0039:
            //输出方式
            if(2 > u16temp)
            {
                ModBusBaseParam->Output_Mode = u16temp;
                InFlash_Write3T_MultiBytes(OUTPUTMODE, &ModBusBaseParam->Output_Mode, 1);
            }
        break;
          
        case 0x003F:
            //量程
            if(u16temp > 0)
            {
                Device_Param->PCap_DataConvert->HeightRange = u16temp;
                InFlash_Write3T_MultiBytes(HEIGHTRANGE, 
                                        (const uint8_t *)&ModBusBaseParam->ModBus_TX_RX.Receive_Buf[7 + Index], 2);
            }
        break;
          
        case 0x0040:
            //空
        break;
          
        case 0x0041:
            //空
        break;
          
        default:
            
        break;
        }     
            
        Index += DataLen;        
    }
}

/**@brief       Modbus 25功能码消息帧处理
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;
* @param[in]    arg : 用户自定义的参数,这里为设备参数
* @return       函数执行结果
* - None
*/
void ModbusFunc25(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg)
{
    //寄存器地址
    uint16_t WriteAddr;
    //数据内容
    uint16_t DataBuf;
    //设备参数
    ModBus_Device_Param *Device_Param;    
    static uint8_t CapAD_ClibFlag = CALIB_CLEAR;
    static uint8_t CapAD_ClibEn = ADCLIB_DISABLE;
    static uint8_t TempDA_ClibFlag = CALIB_CLEAR;
    static uint8_t TempDA_ClibEn = DAOUTCLIB_DISABLE;
        
    Device_Param = (ModBus_Device_Param *)arg;
    
    if(ModBusBaseParam->InRomWrEn != IN_FLASH_WR_ENABLE)
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_DEVC_EXCEPTION;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
        return;
    }

    //写预处理
    ModBus_WritePreHandle(ModBusBaseParam, &WriteAddr, NULL);
    //寄存器地址无效
    if(WriteAddr > _25_FNUC_REG_REGION1_END 
        && (WriteAddr & 0x00B0) != 0x00B0)
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_ADDR_EXCEPTION;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
        return;
    }
    //寄存器内容高字节
    DataBuf = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[4];
    DataBuf <<= 8;
    //寄存器内容低字节
    DataBuf |= ModBusBaseParam->ModBus_TX_RX.Receive_Buf[5];

    //寄存内容无效
    if((0x0000 != DataBuf)&&(0xFF00 != DataBuf))
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_VALU_EXCEPTION;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
        return;
    }

    switch(WriteAddr & ~0x00B0)
    {
    case 0x0000:
    //空
    break;

    case 0x0001:
        //电容DA标定上下刻度线
        if(DAOUTCLIB_ENABLE == Device_Param->PCap_DataConvert->CapDA_ClibEn)
        {
            if(0xFF00 == DataBuf)
            {
                CapDA_ClibFlag |= CALIB_CAPDAHIH_FLAG;
                Calib_CapDAHigh = Device_Param->PCap_DataConvert_Out->PCapDA_ResultValue;
            }
            else if(0x0000 == DataBuf)
            {
                CapDA_ClibFlag |= CALIB_CAPDALOW_FLAG;
                Calib_CapDALow = Device_Param->PCap_DataConvert_Out->PCapDA_ResultValue;
            }
        }
    break;

    case 0x0002:
        //电容DA标定使能
        if(0xFF00 == DataBuf)
        {
            Device_Param->PCap_DataConvert->CapDA_ClibEn = DAOUTCLIB_ENABLE;
        }
        else if(0x0000 == DataBuf)
        {
            Device_Param->PCap_DataConvert->CapDA_ClibEn = DAOUTCLIB_DISABLE;
        }

        if(DAOUTCLIB_DISABLE == Device_Param->PCap_DataConvert->CapDA_ClibEn 
            && (CapDA_ClibFlag & (CALIB_CAPDAMAX_FLAG | CALIB_CAPDAMIN_FLAG)) 
            == (CALIB_CAPDAMAX_FLAG | CALIB_CAPDAMIN_FLAG))
        {
            MB_CapDAOut_Calibration(arg);
            CapDA_ClibFlag = CALIB_CLEAR;
        }
    break;

    case 0x0003:
        //电容DA标定零点满量程
        if(DAOUTCLIB_ENABLE == Device_Param->PCap_DataConvert->CapDA_ClibEn)
        {
            if(0xFF00 == DataBuf)
            {
                CapDA_ClibFlag |= CALIB_CAPDAMAX_FLAG;
                Calib_CapDAMax = Device_Param->PCap_DataConvert_Out->PCapDA_ResultValue;
            }
            else if(0x0000 == DataBuf)
            {
                CapDA_ClibFlag |= CALIB_CAPDAMIN_FLAG;
                Calib_CapDAMin = Device_Param->PCap_DataConvert_Out->PCapDA_ResultValue;
            }
        }
    break;

    case 0x0004:
        //温度标定使能
        if(0xFF00 == DataBuf)
        {
            TempDA_ClibEn = DAOUTCLIB_ENABLE;
        }
        else if(0x0000 == DataBuf)
        {
            TempDA_ClibEn = DAOUTCLIB_DISABLE;
        }

        if((DAOUTCLIB_DISABLE == TempDA_ClibEn) && (CALIB_TEMPDAEOC_FLAG == TempDA_ClibFlag))
        {
            MB_TempDAOut_Calibration(arg);
            TempDA_ClibFlag = CALIB_CLEAR;
        }
    break;

    case 0x0005:
        //温度标定
        if(DAOUTCLIB_ENABLE == TempDA_ClibEn)
        {
            if(0xFF00 == DataBuf)
            {
                TempDA_ClibFlag |= CALIB_TEMPDAMAX_FLAG;
                Calib_TempDAMax = Device_Param->ADC_TemperOut->TemperInAirAD;
            }
            else if(0x0000 == DataBuf)
            {
                TempDA_ClibFlag |= CALIB_TEMPDAMIN_FLAG;
                Calib_TempDAMin = Device_Param->ADC_TemperOut->TemperInAirAD;
            }
        }
    break;

    case 0x0007:
        //电容AD标定使能
        if(0xFF00 == DataBuf)
        {
            CapAD_ClibFlag = CALIB_CLEAR;
            CapAD_ClibEn = ADCLIB_ENABLE;
        }
        else if(0x0000 == DataBuf)
        {
            CapAD_ClibEn = ADCLIB_DISABLE;
        }

        if((ADCLIB_DISABLE == CapAD_ClibEn) && (CALIB_CAPADEOC_FLAG == CapAD_ClibFlag))
        {
            MB_CapAD_Calibration(arg);
            CapAD_ClibFlag = CALIB_CLEAR;
        }
    break;

    case 0x0008:
        //电容AD标定上下刻度线
        if(ADCLIB_ENABLE == CapAD_ClibEn)
        {
            if(0xFF00 == DataBuf)
            {
                Calib_ADHigh = Device_Param->PCap_DataConvert_Out->LiquidHeightAD;
                CapAD_ClibFlag |= CALIB_CAPADHIH_FLAG;
            }
            else if(0x0000 == DataBuf)
            {
                Calib_ADLow = Device_Param->PCap_DataConvert_Out->LiquidHeightAD;
                CapAD_ClibFlag |= CALIB_CAPADLOW_FLAG;
            }
        }
    break;

    case 0x0009:
        //电容AD标定零点满量程
        if(ADCLIB_ENABLE == CapAD_ClibEn)
        {
            if(0xFF00 == DataBuf)
            {
                Calib_ADMax = Device_Param->PCap_DataConvert_Out->LiquidHeightAD;
                CapAD_ClibFlag |= CALIB_CAPADMAX_FLAG;
            }
            else if(0x0000 == DataBuf)
            {
                Calib_ADMin = Device_Param->PCap_DataConvert_Out->LiquidHeightAD;
                CapAD_ClibFlag |= CALIB_CAPADMIN_FLAG;
            }
        }
    break;

    default:
        
    break;
    }
}

/**@brief       Modbus 26功能码消息帧处理
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;
* @param[in]    arg : 用户自定义的参数,这里为设备参数
* @return       函数执行结果
* - None
*/
void ModbusFunc26(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg)
{
    uint8_t i;
    //浮点数据
    float fBuf;
    uint16_t Nr;
    //寄存器地址
    uint16_t ReadAddr;
    //寄存器数量
    uint16_t RegNum;
    //寄存器数据内容
    uint32_t DataBuf;
    //设备参数
    ModBus_Device_Param *Device_Param;
        
    Device_Param = (ModBus_Device_Param *)arg;
    //读预处理
    ModBus_ReadPreHandle(ModBusBaseParam, &ReadAddr, &RegNum);
    //寄存器访问地址无效
    if( ReadAddr < DEF_MUL_REG_REGION1_BEGIN 
        || (ReadAddr + RegNum) > (DEF_MUL_REG_REGION1_END + 2)
        || (RegNum & 0x01) != 0 || (ReadAddr & 0x01) != 0
        || RegNum == 0)
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_ADDR_EXCEPTION;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
        return;
    }

    ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++]
        = RegNum * 2;
    for(Nr = 0; Nr < RegNum; Nr += 2, ReadAddr += 2)
    {
        switch(ReadAddr)
        {
        case 0x0080:
            fBuf = (float)Device_Param->PCap_DataConvert->CapMax;                                       //电容满量程
            DataBuf = *(uint32_t *)&fBuf;
        break;

        case 0x0082:
            fBuf = (float)Device_Param->PCap_DataConvert->CapMin;                                       //电容零点
            DataBuf = *(uint32_t *)&fBuf;
        break;

        case 0x0084:     
            fBuf = (float)Device_Param->PCap_DataConvert->CapDAMax;                                     //DA满度
            DataBuf = *(uint32_t *)&fBuf;
        break;
          
        case 0x0086:
            fBuf = (float)Device_Param->PCap_DataConvert->CapDAHigh;                                     //DA上刻度
            DataBuf = *(uint32_t *)&fBuf;
        break;
          
        case 0x0088:
            fBuf = (float)Device_Param->PCap_DataConvert->CapDALow;                                     //DA下刻度
            DataBuf = *(uint32_t *)&fBuf;
        break;  
          
        case 0x008A:
            fBuf = (float)Device_Param->PCap_DataConvert->CapDAMin;                                     //DA零点
            DataBuf = *(uint32_t *)&fBuf;
        break; 
          
        case 0x008C: 
            fBuf = (float)Device_Param->PCap_DataConvert->CapADMax;                                     //AD满度
            DataBuf = *(uint32_t *)&fBuf;
        break;

        case 0x008E:
            fBuf = (float)Device_Param->PCap_DataConvert->CapADHigh;                                     //AD上刻度
            DataBuf = *(uint32_t *)&fBuf;
        break;       
            
        case 0x0090:
            fBuf = (float)Device_Param->PCap_DataConvert->CapADLow;                                     //AD下刻度
            DataBuf = *(uint32_t *)&fBuf;
        break;  
          
        case 0x0092:
            fBuf = (float)Device_Param->PCap_DataConvert->CapADMin;                                      //AD零点
            DataBuf = *(uint32_t *)&fBuf;
        break;       
          
        case 0x0094:  
            
        break;
          
        case 0x0096:  
            
        break;

        case 0x0098:  
            
        break;

        case 0x009A:  
            
        break;      
          
        case 0x009C:                                                                
            fBuf = Device_Param->ADC_TemperParam->Temper_K1;                                               //环境温度修正系数K1   
            DataBuf = *(uint32_t *)&fBuf;
        break;
          
        case 0x009E:
            fBuf = Device_Param->ADC_TemperParam->Temper_B1;                                               //环境温度修正系数B1
            DataBuf = *(uint32_t *)&fBuf;
        break;
          
        case 0x00A0:
            fBuf = Device_Param->ADC_TemperParam->Temper_K2;                                               //液体温度修正系数K2
            DataBuf = *(uint32_t *)&fBuf;
        break;
          
        case 0x00A2:
            fBuf = Device_Param->ADC_TemperParam->Temper_B2;                                               //液体温度修正系数B2
            DataBuf = *(uint32_t *)&fBuf;
        break;
          
        default:
            
        break;
        }

        for(i = 4; i > 0; i--)
        {
            ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] 
                = (uint8_t)(DataBuf >> ((i - 1)*8));
        }
    }
}

void SwitchCurFilter(uint8_t FiltFactor, DataFilterParam *FilterParam)
{  
    switch(FiltFactor)
    {
    case 0:
        FilterParam->FilterBufMax   = 1;
        FilterParam->FilterCycle    = 1;
    break;

    case 1:
        FilterParam->FilterBufMax   = 1;
        FilterParam->FilterCycle    = 12;
    break;

    case 2:
        FilterParam->FilterBufMax   = 2;
        FilterParam->FilterCycle    = 12;
    break;

    case 3:
        FilterParam->FilterBufMax   = 3;
        FilterParam->FilterCycle    = 12;
    break;

    case 4:
        FilterParam->FilterBufMax   = 4;
        FilterParam->FilterCycle    = 15;
    break;

    case 5:
        FilterParam->FilterBufMax   = 5;
        FilterParam->FilterCycle    = 24;
    break;

    case 6:
        FilterParam->FilterBufMax   = 6;
        FilterParam->FilterCycle    = 30;
    break;

    case 7:
        FilterParam->FilterBufMax   = 8;
        FilterParam->FilterCycle    = 30;
    break;

    case 8:
        FilterParam->FilterBufMax   = 8;
        FilterParam->FilterCycle    = 60;
    break;

    case 9:
        FilterParam->FilterBufMax   = 10;
        FilterParam->FilterCycle    = 96;
    break;

    default:
        FilterParam->FilterBufMax   = 1;
        FilterParam->FilterCycle    = 12;
    break;
    }

    FilterParam->FilterStart = 0;
}

/**@brief       将32位整型变量(大端模式)转换成浮点型值(小端模式)
* @param[in]    pData : 无符号字符型指针
* @return       函数执行结果
* - 转换后的浮点型数值
*/
float HexToFloat(uint8_t *pData)
{
    uint32_t Data;
    Data = pData[0];
    Data <<= 8;
    Data += pData[1];
    Data <<= 8;
    Data += pData[2];
    Data <<= 8;
    Data += pData[3];
    return *(float*)&Data;
}
/**@brief       Modbus 27功能码消息帧处理
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;
* @param[in]    arg : 用户自定义的参数,这里为设备参数
* @return       函数执行结果
* - None
*/
void ModbusFunc27(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg)
{
    //浮点数据
    float fbuf;
    uint16_t Nr;
    //索引
    uint16_t index;
    //寄存器地址
    uint16_t WriteAddr;
    //寄存器数量
    uint16_t RegNum;
    //数据长度
    uint16_t DataLen;
    //设备参数
    ModBus_Device_Param *Device_Param;
    uint8_t Cur_Param[2];
          
    Device_Param = (ModBus_Device_Param *)arg;    
    //写预处理
    ModBus_WritePreHandle(ModBusBaseParam, &WriteAddr, &RegNum);  
    //内部ROM未使能访问失败
    if(IN_FLASH_WR_ENABLE != ModBusBaseParam->InRomWrEn)
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_DEVC_EXCEPTION;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
        return;
    }

    //数据长度
    DataLen = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[6];
    //寄存器访问地址无效
    if( WriteAddr < DEF_MUL_REG_REGION1_BEGIN 
        || (WriteAddr + RegNum) > (DEF_MUL_REG_REGION1_END + 2)
        || (RegNum & 0x01) != 0 || (WriteAddr & 0x01) != 0
        || DataLen != (RegNum * 2)
        || RegNum == 0)
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_ADDR_EXCEPTION;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
        return;
    }
    index = 0;
    for(Nr = 0; Nr < RegNum; Nr += 2, WriteAddr += 2)
    {
        fbuf = HexToFloat(&ModBusBaseParam->ModBus_TX_RX.Receive_Buf[7 + index]);
        switch(WriteAddr)
        {
        case 0x0080:            
            Device_Param->PCap_DataConvert->CapMax = (uint32_t)fbuf;
            Device_Param->DataFilter->InputRangeMax = (uint32_t)fbuf;            
            InFlash_Write3T_MultiBytes(CAPMAX, 
                                    &ModBusBaseParam->ModBus_TX_RX.Receive_Buf[7 + index], 4);
        break;
          
        case 0x0082:
            Device_Param->PCap_DataConvert->CapMin = (uint32_t)fbuf;
            Device_Param->DataFilter->InputRangeMin = (uint32_t)fbuf;
            InFlash_Write3T_MultiBytes(CAPMIN, 
                                    &ModBusBaseParam->ModBus_TX_RX.Receive_Buf[7 + index], 4);
        break;

        case 0x0084:
            if(fbuf <= PCAP_DAC_MAX_VALUE && fbuf >= PCAP_DAC_MIN_VALUE)
            {
                Device_Param->PCap_DataConvert->CapDAMax = (uint16_t)fbuf;              
                Cur_Param[0] = Device_Param->PCap_DataConvert->CapDAMax >> 8;
                Cur_Param[1] = Device_Param->PCap_DataConvert->CapDAMax & 0xFF;
                InFlash_Write3T_MultiBytes(CAPDAMAX, Cur_Param, 2);
            }            
        break;
               
        case 0x0086:
            if(fbuf <= PCAP_DAC_MAX_VALUE && fbuf >= PCAP_DAC_MIN_VALUE)
            {
                Device_Param->PCap_DataConvert->CapDAHigh = (uint16_t)fbuf;       
                Cur_Param[0] = Device_Param->PCap_DataConvert->CapDAHigh >> 8;
                Cur_Param[1] = Device_Param->PCap_DataConvert->CapDAHigh & 0xFF;                
                InFlash_Write3T_MultiBytes(CAPDAHIGH, Cur_Param, 2); 
            }
        break;
          
        case 0x0088:
            if(fbuf <= PCAP_DAC_MAX_VALUE && fbuf >= PCAP_DAC_MIN_VALUE)
            {
                Device_Param->PCap_DataConvert->CapDALow = (uint16_t)fbuf;          
                Cur_Param[0] = Device_Param->PCap_DataConvert->CapDALow >> 8;
                Cur_Param[1] = Device_Param->PCap_DataConvert->CapDALow & 0xFF;
                InFlash_Write3T_MultiBytes(CAPDALOW, Cur_Param, 2); 
            }      
        break;
              
        case 0x008A:
            if(fbuf <= PCAP_DAC_MAX_VALUE && fbuf >= PCAP_DAC_MIN_VALUE)
            {
                Device_Param->PCap_DataConvert->CapDAMin = (uint16_t)fbuf;     
                Cur_Param[0] = Device_Param->PCap_DataConvert->CapDAMin >> 8;
                Cur_Param[1] = Device_Param->PCap_DataConvert->CapDAMin & 0xFF;
                InFlash_Write3T_MultiBytes(CAPDAMIN, Cur_Param, 2);
            }     
        break; 
          
        case 0x008C:
            if(fbuf <= PCAP_ADC_MAX_VALUE && fbuf >= PCAP_ADC_MIN_VALUE)
            {
                Device_Param->PCap_DataConvert->CapADMax = (uint16_t)fbuf;    
                Cur_Param[0] = Device_Param->PCap_DataConvert->CapADMax >> 8;
                Cur_Param[1] = Device_Param->PCap_DataConvert->CapADMax & 0xFF;
                InFlash_Write3T_MultiBytes(CAPADMAX, Cur_Param, 2);      
            }            
        break; 
               
        case 0x008E:
            if(fbuf <= PCAP_ADC_MAX_VALUE && fbuf >= PCAP_ADC_MIN_VALUE)
            {
                Device_Param->PCap_DataConvert->CapADHigh = (uint16_t)fbuf;       
                Cur_Param[0] = Device_Param->PCap_DataConvert->CapADHigh >> 8;
                Cur_Param[1] = Device_Param->PCap_DataConvert->CapADHigh & 0xFF;
                InFlash_Write3T_MultiBytes(CAPADHIGH, Cur_Param, 2);       
            }                
        break; 
          
        case 0x0090:
            if(fbuf <= PCAP_ADC_MAX_VALUE && fbuf >= PCAP_ADC_MIN_VALUE)
            {
                Device_Param->PCap_DataConvert->CapADLow = (uint16_t)fbuf;        
                Cur_Param[0] = Device_Param->PCap_DataConvert->CapADLow >> 8;
                Cur_Param[1] = Device_Param->PCap_DataConvert->CapADLow & 0xFF;
                InFlash_Write3T_MultiBytes(CAPADLOW, Cur_Param, 2);           
            }
        break; 
               
        case 0x0092:
            if(fbuf <= PCAP_ADC_MAX_VALUE && fbuf >= PCAP_ADC_MIN_VALUE)
            {
                Device_Param->PCap_DataConvert->CapADMin = (uint16_t)fbuf;        
                Cur_Param[0] = Device_Param->PCap_DataConvert->CapADMin >> 8;
                Cur_Param[1] = Device_Param->PCap_DataConvert->CapADMin & 0xFF;
                InFlash_Write3T_MultiBytes(CAPADMIN, Cur_Param, 2);   
            }            
        break;       

        default:
            
        break;
        }
        index += DataLen;
    }
}

/**@brief       Modbus 2A功能码消息帧处理
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;
* @return       函数执行结果
* - None
*/
void ModbusFunc2A(ModBusBaseParam_TypeDef *ModBusBaseParam)
{
    uint8_t i;
    uint8_t j;
    uint8_t objlen;
    uint16_t RegNum;
    uint16_t WriteAddr;

    //写预处理
    ModBus_WritePreHandle(ModBusBaseParam, &WriteAddr, &RegNum);    
    if( WriteAddr < MUL_VERSION_INF_BEGIN 
        || (WriteAddr + RegNum) > (MUL_VERSION_INF_END + 1)
        || RegNum == 0)
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_ADDR_EXCEPTION;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
        return;
    }

    j = 6;
    for(i = 0; i < RegNum; i++, WriteAddr++)
    {
        objlen = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[j] + 1;
        switch(WriteAddr)
        {
        //机构名称
        case 0x00E0:            
            if(0x2F < objlen)
            {
                j += objlen;
                break;
            }
            InFlash_Write_MultiBytes(ORGANIZATION, 
                (const uint8_t *)&ModBusBaseParam->ModBus_TX_RX.Receive_Buf[j], objlen);                                    
            j += objlen;
        break;
        //产品代号    
        case 0x00E1:
            if(0x2F < objlen)
            {
                j += objlen;
                break;
            }
            InFlash_Write_MultiBytes(PRODUCTION, 
                (const uint8_t *)&ModBusBaseParam->ModBus_TX_RX.Receive_Buf[j], objlen);                               
            j += objlen;
        break;
        //硬件版本    
        case 0x00E2:
            if(0x2F < objlen)
            {
                j += objlen;
                break;
            }
            InFlash_Write_MultiBytes(HARDWAREVER, 
                (const uint8_t *)&ModBusBaseParam->ModBus_TX_RX.Receive_Buf[j], objlen);                                  
            j += objlen;
        break;
        //软件版本    
        case 0x00E3:
            if(0x2F < objlen)
            {
                j += objlen;
                break;
            }
            InFlash_Write_MultiBytes(SOFTWAREVER, 
                (const uint8_t *)&ModBusBaseParam->ModBus_TX_RX.Receive_Buf[j], objlen);                                  
            j += objlen;
        break;
        //设备ID    
        case 0x00E4:
            if(0x2F < objlen)
            {
                j += objlen;
                break;
            }
            InFlash_Write_MultiBytes(DEVICENUM, 
                (const uint8_t *)&ModBusBaseParam->ModBus_TX_RX.Receive_Buf[j], objlen);                                     
            j += objlen;
        break;
        //客户编码    
        case 0x00E5:
            if(0x2F < objlen)
            {
                j += objlen;
                break;
            }
            InFlash_Write_MultiBytes(CUSTOMER, 
                (const uint8_t *)&ModBusBaseParam->ModBus_TX_RX.Receive_Buf[j], objlen);                               
            j += objlen;
        break;
            
        default:
            
        break;
        }        
    }
}

/**@brief       Modbus 2B功能码消息帧处理
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;
* @return       函数执行结果
* - None
*/
void ModbusFunc2B(ModBusBaseParam_TypeDef *ModBusBaseParam)
{
    uint8_t i;
    uint8_t objlen;
    uint16_t RegNum;
    uint16_t ReadAddr;

    //读预处理
    ModBus_ReadPreHandle(ModBusBaseParam, &ReadAddr, &RegNum);    
    
    if( ReadAddr < MUL_VERSION_INF_BEGIN
        || (ReadAddr + RegNum) > (MUL_VERSION_INF_END + 1)
        || RegNum == 0)
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] |= MB_REQ_FAILURE;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_ADDR_EXCEPTION;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
        return;
    }
    
    memcpy(&ModBusBaseParam->ModBus_TX_RX.Send_Buf[2], &ModBusBaseParam->ModBus_TX_RX.Receive_Buf[4], 2);
    memcpy(&ModBusBaseParam->ModBus_TX_RX.Send_Buf[4], &ModBusBaseParam->ModBus_TX_RX.Receive_Buf[2], 2);
    ModBusBaseParam->ModBus_TX_RX.Send_Len = 6;
    for(i = 0; i < RegNum; i++, ReadAddr++)
    {
        switch(ReadAddr)
        {
        //机构名称  
        case 0x00E0:
            objlen = InFlash_Read_OneByte(ORGANIZATION);                                                       
            ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] = objlen;
            if((objlen > (SEND_SIZE / 3))||(0 == objlen))
            {
                ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len - 1] = 1;
                ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] = 0;
                break;
            }
            InFlash_Read_MultiBytes((ORGANIZATION + 1), 
                &ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len], objlen);
            ModBusBaseParam->ModBus_TX_RX.Send_Len += objlen;
        break;
        //产品代号  
        case 0x00E1:
            objlen = InFlash_Read_OneByte(PRODUCTION);                                                  
            ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] = objlen;
            if((objlen > (SEND_SIZE / 3))||(0 == objlen))
            {
                ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len - 1] = 1;
                ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] = 0;
                break;
            }
            InFlash_Read_MultiBytes((PRODUCTION + 1), 
                &ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len], objlen);
            ModBusBaseParam->ModBus_TX_RX.Send_Len += objlen;
        break;
        //硬件版本  
        case 0x00E2:
            objlen = InFlash_Read_OneByte(HARDWAREVER);                                                     
            ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] = objlen;
            if((objlen > (SEND_SIZE / 3))||(0 == objlen))
            {
                ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len - 1] = 1;
                ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] = 0;
                break;
            }
            InFlash_Read_MultiBytes((HARDWAREVER + 1), 
                &ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len], objlen);
            ModBusBaseParam->ModBus_TX_RX.Send_Len += objlen;
        break;
        //软件版本 
        case 0x00E3:                        
            objlen = InFlash_Read_OneByte(SOFTWAREVER);
            ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] = objlen;
            if((objlen > (SEND_SIZE / 3))||(0 == objlen))
            {
                ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len - 1] = 1;
                ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] = 0;
                break;
            }
            InFlash_Read_MultiBytes((SOFTWAREVER + 1), 
                &ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len], objlen);
            ModBusBaseParam->ModBus_TX_RX.Send_Len += objlen;
        break;
        //设备ID  
        case 0x00E4:
            objlen = InFlash_Read_OneByte(DEVICENUM);                                                        
            ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] = objlen;
            if((objlen > (SEND_SIZE / 3))||(0 == objlen))
            {
                ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len - 1] = 1;
                ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] = 0;
                break;
            }
            InFlash_Read_MultiBytes((DEVICENUM + 1), 
                &ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len], objlen);
            ModBusBaseParam->ModBus_TX_RX.Send_Len += objlen;
        break;
        //客户编码  
        case 0x00E5:
            objlen = InFlash_Read_OneByte(CUSTOMER);                                                  
            ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] = objlen;
            if((objlen > (SEND_SIZE / 3))||(0 == objlen))
            {
                ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len - 1] = 1;
                ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] = 0;
                break;
            }
            InFlash_Read_MultiBytes((CUSTOMER + 1), 
                &ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len], objlen);
            ModBusBaseParam->ModBus_TX_RX.Send_Len += objlen;
        break;

        default:
            
        break;
        }        
    }
}

/**@brief       Modbus 41功能码消息帧处理
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;
* @return       函数执行结果
* - None
*/
void ModbusFunc41(ModBusBaseParam_TypeDef *ModBusBaseParam)
{
    uint16_t temp[2];
    
    temp[0] = ((uint16_t)ModBusBaseParam->ModBus_TX_RX.Receive_Buf[2] << 8) 
                | ModBusBaseParam->ModBus_TX_RX.Receive_Buf[3];
    temp[1] = ((uint16_t)ModBusBaseParam->ModBus_TX_RX.Receive_Buf[4] << 8) 
                | ModBusBaseParam->ModBus_TX_RX.Receive_Buf[5];
    if(temp[0] != 0x0001 || temp[1] != 0x0000)
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[0] = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[0];
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] = ModBusBaseParam->ModBus_TX_RX.Receive_Buf[1] 
                                                    | MB_REQ_FAILURE;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = MB_VALU_EXCEPTION;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
    }
    else
    {
        memcpy(ModBusBaseParam->ModBus_TX_RX.Send_Buf, ModBusBaseParam->ModBus_TX_RX.Receive_Buf, 5);
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[5] = 0x01;
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[6] = 0x00;
        ModBusBaseParam->ModBus_TX_RX.Send_Len = 7;        
        ModBusBaseParam->ModBus_CallBack = MB_System_Reset;
    }
}

/**@brief       Modbus 消息帧自动上传处理
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;
* @param[in]    arg : 用户自定义的参数,这里为设备参数
* @return       函数执行结果
* - None
*/
void ModbusAutoUpload(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg)
{
    float fbuf;
    uint8_t Nr;
    uint32_t DataBuf;
    //设备参数
    ModBus_Device_Param *Device_Param;
        
    Device_Param = (ModBus_Device_Param *)arg;

    ModBusBaseParam->ModBus_TX_RX.Send_Buf[0] = ModBusBaseParam->Device_Addr;
    ModBusBaseParam->ModBus_TX_RX.Send_Buf[1] = 0x04;
    ModBusBaseParam->ModBus_TX_RX.Send_Buf[2] = 0x08;

    ModBusBaseParam->ModBus_TX_RX.Send_Len = 3;
    fbuf = (float)Device_Param->PCap_DataConvert_Out->LiquidHeightAD;
    DataBuf = *(uint32_t *)&fbuf;
    for(Nr = 4; Nr > 0; Nr--)
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] 
            = (uint8_t)(DataBuf >> ((Nr - 1)*8));
    }

    ModBusBaseParam->ModBus_TX_RX.Send_Len = 7;
    DataBuf = *(uint32_t *)&Device_Param->ADC_TemperOut->TemperInAir;
    for(Nr = 4; Nr > 0; Nr--)
    {
        ModBusBaseParam->ModBus_TX_RX.Send_Buf[ModBusBaseParam->ModBus_TX_RX.Send_Len++] 
            = (uint8_t)(DataBuf >> ((Nr - 1)*8));
    }
    MDB_Snd_Msg_RTU(ModBusBaseParam, 
                    ModBusBaseParam->ModBus_TX_RX.Send_Buf, 
                    ModBusBaseParam->ModBus_TX_RX.Send_Len,
                    NO_CHECK_ADDRESS);
}

/**@brief       电容标定
* @param[in]    arg : 用户自定义的参数,这里为设备参数
* @return       函数执行结果
* - None
*/
void MB_Cap_Calibration(void *arg)
{
    uint8_t Cur_Param[8];
    //设备参数
    ModBus_Device_Param *Device_Param;
        
    Device_Param = (ModBus_Device_Param *)arg;
    
    if(Calib_CapMin < Calib_CapMax)
    {
        //电容零点存入缓存
        Cur_Param[0] = (uint8_t)(Calib_CapMin >> 24);
        Cur_Param[1] = (uint8_t)(Calib_CapMin >> 16);
        Cur_Param[2] = (uint8_t)(Calib_CapMin >> 8);
        Cur_Param[3] = (uint8_t)Calib_CapMin;
        //电容满点存入缓存
        Cur_Param[4] = (uint8_t)(Calib_CapMax >> 24);
        Cur_Param[5] = (uint8_t)(Calib_CapMax >> 16);
        Cur_Param[6] = (uint8_t)(Calib_CapMax >> 8);
        Cur_Param[7] = (uint8_t)Calib_CapMax;
        //电容零点
        Device_Param->PCap_DataConvert->CapMin = Calib_CapMin;
        //电容满点
        Device_Param->PCap_DataConvert->CapMax = Calib_CapMax;
        
        Device_Param->DataFilter->InputRangeMin = Calib_CapMin;
        Device_Param->DataFilter->InputRangeMax = Calib_CapMax;
        //电容标定数据写入内部Flash
        InFlash_Write3T_MultiBytes(CAPMIN, Cur_Param, 8);
        //如果电容满点备份值是缺省值就写入标定值
        if(0xFFFFFFFF == Device_Param->PCap_DataConvert->CapMaxBak)
        {
            //电容备份数据写入内部Flash
            InFlash_Write3T_MultiBytes(CAPMINBAK, Cur_Param, 8);
            //电容零点备份
            Device_Param->PCap_DataConvert->CapMinBak = Calib_CapMin;
            //电容满点备份
            Device_Param->PCap_DataConvert->CapMaxBak = Calib_CapMax;
        }
    }
    //Device_Param->PCap_DataConvert->CapRange = Device_Param->PCap_DataConvert->CapMax - Device_Param->PCap_DataConvert->CapMin;
    //设置液位波动阈值为量程的百分之三
    //Device_Param->PCap_DataConvert->CapWaveLimit = (uint32_t)(Device_Param->PCap_DataConvert->CapRange * 0.03);
}

/**@brief       电容AD值标定
* @param[in]    arg : 用户自定义的参数,这里为设备参数
* @return       函数执行结果
* - None
*/
void MB_CapAD_Calibration(void *arg)
{
    uint8_t Cur_Param[8];
    //设备参数
    ModBus_Device_Param *Device_Param;
        
    Device_Param = (ModBus_Device_Param *)arg;
    //如果电容AD标定值有效就写入内部Flash
    if((Calib_ADMin < Calib_ADLow) && (Calib_ADLow < Calib_ADHigh) && (Calib_ADHigh < Calib_ADMax))
    {
        Cur_Param[0] = (uint8_t)(Calib_ADMin >> 8);
        Cur_Param[1] = (uint8_t)Calib_ADMin;
        Cur_Param[2] = (uint8_t)(Calib_ADLow >> 8);
        Cur_Param[3] = (uint8_t)Calib_ADLow;
        Cur_Param[4] = (uint8_t)(Calib_ADHigh >> 8);
        Cur_Param[5] = (uint8_t)Calib_ADHigh;
        Cur_Param[6] = (uint8_t)(Calib_ADMax >> 8);
        Cur_Param[7] = (uint8_t)Calib_ADMax;

        Device_Param->PCap_DataConvert->CapADMin = Calib_ADMin;
        Device_Param->PCap_DataConvert->CapADLow = Calib_ADLow;
        Device_Param->PCap_DataConvert->CapADHigh = Calib_ADHigh;
        Device_Param->PCap_DataConvert->CapADMax = Calib_ADMax;
        InFlash_Write3T_MultiBytes(CAPADMIN, Cur_Param, 8);
    }
}

/**@brief       电容DA值标定
* @param[in]    arg : 用户自定义的参数,这里为设备参数
* @return       函数执行结果
* - None
*/
void MB_CapDAOut_Calibration(void *arg)
{
    uint8_t Cur_Param[8];
    //设备参数
    ModBus_Device_Param *Device_Param;
    
    Device_Param = (ModBus_Device_Param *)arg;
    if(CapDA_ClibFlag == (CALIB_CAPDAMAX_FLAG | CALIB_CAPDAMIN_FLAG))
    {
        if(Calib_CapDAMax <=  Calib_CapDAMin)
        {
            return;
        }
        Calib_CapDAHigh = 0;
        Calib_CapDALow = 0;        
    }
    else
    {
        if((Calib_CapDAMax <= Calib_CapDAHigh) || (Calib_CapDAHigh <= Calib_CapDALow) 
            || (Calib_CapDALow <= Calib_CapDAMin))
        {
            return;
        }
    }                    
    Cur_Param[0] = (uint8_t)(Calib_CapDAMin >> 8);
    Cur_Param[1] = (uint8_t)Calib_CapDAMin;
    Cur_Param[2] = (uint8_t)(Calib_CapDALow >> 8);
    Cur_Param[3] = (uint8_t)Calib_CapDALow;
    Cur_Param[4] = (uint8_t)(Calib_CapDAHigh >> 8);
    Cur_Param[5] = (uint8_t)Calib_CapDAHigh;
    Cur_Param[6] = (uint8_t)(Calib_CapDAMax >> 8);
    Cur_Param[7] = (uint8_t)Calib_CapDAMax;
    InFlash_Write3T_MultiBytes(CAPDAMIN, Cur_Param, 8);
    
    Device_Param->PCap_DataConvert->CapDAMin = Calib_CapDAMin;
    Device_Param->PCap_DataConvert->CapDALow = Calib_CapDALow;
    Device_Param->PCap_DataConvert->CapDAHigh = Calib_CapDAHigh;
    Device_Param->PCap_DataConvert->CapDAMax = Calib_CapDAMax;
}

/**@brief       温度DA值标定功能
* @param[in]    arg : 用户自定义的参数,这里为设备参数
* @return       函数执行结果
* - None
*/
void MB_TempDAOut_Calibration(void *arg)
{
    uint8_t Cur_Param[4];
    //设备参数
    ModBus_Device_Param *Device_Param;
    
    Device_Param = (ModBus_Device_Param *)arg;
    //如果温度DA标定值有效就写入内部Flash
    if((Calib_TempDAMax > Calib_TempDAMin) && (Calib_TempDAMax <= DAC_VALUE_MAX))
    {
        Cur_Param[0] = (uint8_t)(Calib_TempDAMin >> 8);
        Cur_Param[1] = (uint8_t)Calib_TempDAMin;
        Cur_Param[2] = (uint8_t)(Calib_TempDAMax >> 8);
        Cur_Param[3] = (uint8_t)Calib_TempDAMax;
        InFlash_Write3T_MultiBytes(TEMPDAMIN, Cur_Param, 4);

        Device_Param->ADC_TemperParam->TempDAMin = Calib_TempDAMin;
        Device_Param->ADC_TemperParam->TempDAMax = Calib_TempDAMax;
        Device_Param->ADC_TemperParam->TempDARange = Device_Param->ADC_TemperParam->TempDAMax 
                                                - Device_Param->ADC_TemperParam->TempDAMin;
    }
}

/**@brief       Modbus 串口重新初始化
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;  
* @return       函数执行结果
* - int整型值(OP_SUCCESS)
* @note         本函数赋给ModBusBaseParam->ModBus_CallBack后,可在ModBus响应上位机后调用
*/
static int MB_USART_ReInit(ModBusBaseParam_TypeDef *ModBusBaseParam)
{        
/* 使用RT-Thread操作系统,USING_RT_THREAD_OS在main.h中定义 */
#ifdef USING_RT_THREAD_OS
    rt_sem_take(ModBusBaseParam->TX_Lock, RT_WAITING_FOREVER);    //获取信号量
#else
    while(Sensor_USART_Get_TX_Cplt_Flag() == 0);    //等待串口发送完毕
#endif
    
    Sensor_USART_Init(  ModBusBaseParam->BaudRate, 
                        ModBusBaseParam->Parity);
    
/* 使用RT-Thread操作系统,USING_RT_THREAD_OS在main.h中定义 */
#ifdef USING_RT_THREAD_OS
    rt_sem_release(ModBusBaseParam->TX_Lock);    //释放信号量
#endif  
    return OP_SUCCESS;
}

/**@brief       Modbus 设备冻结
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;  
* @return       函数执行结果
* - int整型值(OP_SUCCESS)
* @note         本函数赋给ModBusBaseParam->ModBus_CallBack后,可在ModBus响应上位机后调用
*/
static int MB_Freeze(ModBusBaseParam_TypeDef *ModBusBaseParam)
{        
    ModBusBaseParam->Freeze = FREEZE_ENABLE;
    
    return OP_SUCCESS;
}

/**@brief       系统复位
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;  
* @return       函数执行结果
* - int整型值(OP_SUCCESS)
* @note         本函数赋给ModBusBaseParam->ModBus_CallBack后,可在ModBus响应上位机后调用
*/
static int MB_System_Reset(ModBusBaseParam_TypeDef *ModBusBaseParam)
{
/* 使用RT-Thread操作系统,USING_RT_THREAD_OS在main.h中定义 */
#ifdef USING_RT_THREAD_OS
    rt_sem_take(ModBusBaseParam->TX_Lock, RT_WAITING_FOREVER);    //获取信号量
#else
    while(Sensor_USART_Get_TX_Cplt_Flag() == 0);    //等待串口发送完毕
#endif
    
    HAL_NVIC_SystemReset();
    
    return OP_SUCCESS;
}

/**@brief       发送ModBus数据且不检查设备地址
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;  
* @return       函数执行结果
* - int整型值(OP_SUCCESS)
* @note         本函数赋给ModBusBaseParam->ModBus_CallBack后,可在ModBus响应上位机后调用
*/
static int MB_SendData_NoCheck(ModBusBaseParam_TypeDef *ModBusBaseParam)
{
    //发送Modbus RTU
    MDB_Snd_Msg_RTU(ModBusBaseParam, 
                    ModBusBaseParam->ModBus_TX_RX.Send_Buf, 
                    ModBusBaseParam->ModBus_TX_RX.Send_Len,
                    NO_CHECK_ADDRESS);
    
    return OP_SUCCESS;
}

#endif
