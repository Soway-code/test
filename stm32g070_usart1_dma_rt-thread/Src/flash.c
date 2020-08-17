
#include "flash.h"

uint32_t FirstPage = 0, NbOfPages = 0;
uint32_t Address = 0,PageError = 0;
FLASH_EraseInitTypeDef EraseInitStruct = {0};


uint32_t GetPage(uint32_t Addr)
{
  return (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
}

/**@brief       擦除一个区域的空间
* @param[in]    Address 擦除地址
* @return       函数执行结果
*/
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


/**@brief       写入一个双字节的数据
* @param[in]    Address 写入的地址
* @param[in]   *Data 	要写入数据的首地址
* @param[in] 	DataLen	写入数据的长度
* @return       函数执行结果
*/
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

