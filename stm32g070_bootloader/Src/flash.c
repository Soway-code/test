#include "flash.h"

extern FLASH_ProcessTypeDef pFlash;
FLASH_EraseInitTypeDef EraseInitStruct = {0};
uint32_t GetPage(uint32_t Addr);
uint32_t FirstPage = 0, NbOfPages = 0;
uint32_t Address = 0, PageError = 0;
#define FLASH_USER_END_ADDR     (FLASH_BASE + FLASH_SIZE - 1)   /* End @ of user Flash area */
//******************************************************************************
// 名称               : FlashLastPageErase()
// 创建日期           : 2019.6.13
// 作者               : 
// 功能               : 擦除Flash一个扇区
// 输入参数           : 无
// 输出参数           : 擦除状态标志
// 返回结果           : 无
//******************************************************************************
HAL_StatusTypeDef FlashPageErase(uint32_t Address)								// 擦除Flash一个扇区
{
	HAL_StatusTypeDef status = HAL_OK;
	FirstPage=GetPage(Address);
	NbOfPages=1;
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page        = FirstPage;
    EraseInitStruct.NbPages     = NbOfPages;
	
	HAL_FLASH_Unlock();
    HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
//	HAL_FLASH_Lock();

	
	return status;
}


//******************************************************************************
// 名称               : FlashReadNHalfWord()
// 创建日期           : 2019.6.13
// 作者               : 
// 功能               : 读取Flash N个半字
// 输入参数           : ReadAddr -- 读取Flash首地址
//						pBuffer -- 读取Flash数据缓冲地址
//						NumToRead -- 读取半字个数
// 输出参数           : 无
// 返回结果           : 无
// 注意和说明         : 无
// 修改内容           : 无
//******************************************************************************
void FlashReadNHalfWord(uint32_t ReadAddr, uint16_t *pBuffer, uint16_t NumToRead)	// 读取N个半字
{
	uint16_t i;
	
    HAL_FLASH_Unlock();
	
	for(i = 0; i < NumToRead; i++)
	{
		pBuffer[i] = *(uint16_t*)(ReadAddr);			// 读取2个字节.
		ReadAddr += 2;									// 偏移2个字节.	
	}
	
	HAL_FLASH_Lock();
}

//******************************************************************************
// 名称               : FlashReadNWord()
// 创建日期           : 2019.6.13
// 作者               : 
// 功能               : 读取Flash N个字
// 输入参数           : ReadAddr -- 读取数据首地址
//						pBuffer -- 读取Flash数据缓冲地址
//						NumToRead -- 读取字个数
// 输出参数           : 无
// 返回结果           : 无
// 注意和说明         : 无
// 修改内容           : 无
//******************************************************************************
void FlashReadNWord(uint32_t ReadAddr, uint32_t *pBuffer, uint16_t NumToRead)		// 读取N个字
{
	uint32_t i;
	
    HAL_FLASH_Unlock();
	
	for(i = 0; i < NumToRead; i++)
	{
		pBuffer[i] = FlashReadWord(ReadAddr);			// 读取4个字节.
		ReadAddr += 4;									// 偏移4个字节.	
	}
	
	HAL_FLASH_Lock();
}

//******************************************************************************
// 名称               : FlashReadWord()
// 创建日期           : 2019.6.13
// 作者               : 
// 功能               : 读取Flash 1个字
// 输入参数           : addr -- 读取数据地址
// 输出参数           : addr地址数据
// 返回结果           : 无
// 注意和说明         : 无
// 修改内容           : 无
//******************************************************************************
uint32_t FlashReadWord(uint32_t addr)					// 读一个字
{
	return *(uint32_t*)addr; 
}

//******************************************************************************
// 名称               : FlashReadHalfWord()
// 创建日期           : 2019.6.13
// 作者               : 
// 功能               : 读取Flash 1个半字
// 输入参数           : addr -- 读取数据地址
// 输出参数           : addr地址数据
// 返回结果           : 无
// 注意和说明         : 无
// 修改内容           : 无
//******************************************************************************
uint16_t FlashReadHalfWord(uint32_t addr)				// 读半字
{
	return *(uint16_t*)addr; 
}

//******************************************************************************
// 名称               : FlashLastPageErase()
// 创建日期           : 2019.6.13
// 作者               : 
// 功能               : 擦除Flash最后一个扇区
// 输入参数           : 无
// 输出参数           : 擦除状态标志
// 返回结果           : 无
// 注意和说明         : 无
// 修改内容           : 无
//******************************************************************************
HAL_StatusTypeDef FlashLastPageErase(void)				// 擦除Flash最后一个扇区
{
	HAL_StatusTypeDef status = HAL_OK;
	FirstPage=GetPage(FLASH_LAST_PAGE_ADDR);
	NbOfPages=1;
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page        = FirstPage;
    EraseInitStruct.NbPages     = NbOfPages;
	DIS_INT;
	HAL_FLASH_Unlock();

	status = FLASH_WaitForLastOperation(0xFF);			// 等待操作完成
	
	if(status == HAL_OK)
	{
		/* Clean the error context */
		pFlash.ErrorCode = HAL_FLASH_ERROR_NONE;

		/* Proceed to erase the page */

		
        HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
		
	
	}
	
	HAL_FLASH_Lock();
	EN_INT;
	
	return status;
}

//******************************************************************************
// 名称               : FlashWriteHalfWord()
// 创建日期           : 2019.6.13
// 作者               : 
// 功能               : Flash 写1个半字
// 输入参数           : Address -- 写数据地址
// 输出参数           : 无
// 返回结果           : 无
// 注意和说明         : 无
// 修改内容           : 无
//******************************************************************************
void FlashWriteHalfWord(uint32_t Address, uint16_t Data)							// 写半字
{
	HAL_FLASH_Unlock();
	DIS_INT;
	
	SET_BIT(FLASH->CR, FLASH_CR_PG);
	*(__IO uint16_t*)Address = Data;

	HAL_FLASH_Lock();

	EN_INT;
}


//******************************************************************************
// 名称               : FlashWriteNHalfWord()
// 创建日期           : 2019.6.13
// 作者               : 
// 功能               : Flash 写N个半字
// 输入参数           : Address -- 写数据首地址
//						Data -- 写数据指针
//						DataLen -- 写数据长度
// 输出参数           : 无
// 返回结果           : 无
// 注意和说明         : 无
// 修改内容           : 无
//******************************************************************************
uint8_t FlashWriteDoubleWord(uint32_t Address, uint64_t *Data, uint16_t DataLen)		// 写N个双字
{
	
	uint8_t status=0;
	uint16_t i =0;
    HAL_FLASH_Unlock();
	for(i = 0; i < DataLen; i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (Address + (i * 8)), Data[i]);
	
	}
 
	return status;

}

uint32_t FLASH_If_Erase(uint32_t StartSector)
{
	HAL_StatusTypeDef status = HAL_OK;
	FirstPage=GetPage(StartSector);
	NbOfPages = GetPage(FLASH_USER_END_ADDR) - FirstPage + 1;
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page        = FirstPage;
    EraseInitStruct.NbPages     = NbOfPages;
	
	HAL_FLASH_Unlock();
    HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
//	HAL_FLASH_Lock();
    return 	status;
}





uint32_t GetPage(uint32_t Addr)
{
  return (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;;
}




