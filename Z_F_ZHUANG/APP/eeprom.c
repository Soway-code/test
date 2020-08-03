#include "eeprom.h"
//#include "para.h"
/*
void Eeprom_WriteByte(uint16_t pWriteAddr, uint8_t pData)
{
    uint32_t WriteAddr;
    uint32_t TimeOut = 0xffffff;
    if(pWriteAddr > EEPROM_BYTE_SIZE)
    {
        return;
    }
    WriteAddr = EEPROM_BASE_ADDR + pWriteAddr;
    HAL_FLASH_Unlock();
    //DIS_INT;
    *(uint8_t *)WriteAddr = pData;
    while(__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY) && TimeOut > 0)
    {
        TimeOut--;
    }
    HAL_FLASH_Lock();
    //EN_INT;       
}


void Eeprom_ReadByte(uint16_t pReadAddr, uint8_t *pReadData)
{
    uint32_t ReadAddr;
    if(pReadAddr > EEPROM_BYTE_SIZE)
    {
        return;
    }
    ReadAddr = EEPROM_BASE_ADDR + pReadAddr;
    //DIS_INT;
    *pReadData = *(uint8_t *)ReadAddr;
    //EN_INT;
}

*/
/*
void Eeprom_Write_OneByte(uint16_t EepAddr, uint8_t WrData)
{
  uint32_t WrAddr;

  WrAddr = 0x08080000;
  WrAddr |= EepAddr;
  assert_param(IS_FLASH_DATA_ADDRESS(WrAddr));
  HAL_FLASHEx_DATAEEPROM_Unlock();
  FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);

  __disable_irq();
  HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE, WrAddr, WrData);
  FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
  HAL_FLASHEx_DATAEEPROM_Lock();
  FLASH_WaitForLastOperation(FLASH_TIMEOUT_VALUE);
  __enable_irq();

}
*/


//******************************************************************************
// 名称         : Eeprom_WriteNBytes()
// 创建日期     : 2018-06-08
// 作者         : MMX
// 功能         : 向EEPROM中写入N字节数据
// 输入参数     : uint16_t pWriteAddr       写地址
//                uint8_t *pBuffer          写入的数据
//                uint16_t pLen             写入的数据长度  
// 输出参数     : 无
// 返回结果     : 无
// 注意和说明   : 无
// 修改内容     : 无 
//******************************************************************************
void Eeprom_WriteNBytes(uint16_t pWriteAddr, uint8_t *pBuffer, uint16_t pLen)
{
    uint32_t WriteAddr;
    uint32_t TimeOut = 0xffff;
    
    if(pWriteAddr > EEPROM_BYTE_SIZE)
    {
        return;
    }
    WriteAddr = EEPROM_BASE_ADDR + pWriteAddr;
//    DIS_INT;
//    HAL_FLASHEx_DATAEEPROM_Unlock();
	HAL_FLASH_Unlock();
    while(pLen--)
    {
        *(uint8_t *)WriteAddr++ = *pBuffer++;
        while(__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY) && TimeOut > 0)
        {
            TimeOut--;
        }
    }
//    HAL_FLASHEx_DATAEEPROM_Lock();
	HAL_FLASH_Lock();
//    EN_INT;
}




//******************************************************************************
// 名称         : Eeprom_ReadNBytes()
// 创建日期     : 2018-06-08
// 作者         : MMX
// 功能         : 从EEPROM中读取N字节数据
// 输入参数     : uint16_t pReadAddr        读地址
//                uint8_t *pBuffer          读取数据缓存
//                uint16_t pLen             读数据长度      
// 输出参数     : 无
// 返回结果     : 无
// 注意和说明   : 无
// 修改内容     : 无 
//******************************************************************************
void Eeprom_ReadNBytes(uint16_t pReadAddr, uint8_t *pBuffer, uint16_t pLen)
{
    uint32_t ReadAddr;
    if(pReadAddr > EEPROM_BYTE_SIZE)
    {
        return;
    }
    ReadAddr = EEPROM_BASE_ADDR + pReadAddr;
//    DIS_INT;
    while(pLen--)
    {
        *pBuffer++ = *(uint8_t *)ReadAddr++;
    }
//    EN_INT;
}

/*
void Eeprom_WNBytesMul3T(uint16_t WriteAddr, uint8_t *pBuffer, uint16_t NumToWrite)
{
    Eeprom_WriteNBytes(WriteAddr, pBuffer, NumToWrite);
    Eeprom_WriteNBytes(WriteAddr + 2 * 0x80, pBuffer, NumToWrite);
    Eeprom_WriteNBytes(WriteAddr + 4 * 0x80, pBuffer, NumToWrite);
}
*/





