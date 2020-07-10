#include "flash.h"

void FLASH_If_Init(void)
{
  /* Unlock the Program memory */
  HAL_FLASH_Unlock();
///  *     @arg @ref FLASH_FLAG_OPERR FLASH Operation error flag
///  *     @arg @ref FLASH_FLAG_PROGERR FLASH Programming error flag
///  *     @arg @ref FLASH_FLAG_WRPERR FLASH Write protection error flag
///  *     @arg @ref FLASH_FLAG_PGAERR FLASH Programming alignment error flag
///  *     @arg @ref FLASH_FLAG_SIZERR FLASH Size error flag
///  *     @arg @ref FLASH_FLAG_PGSERR FLASH Programming sequence error flag
///  *     @arg @ref FLASH_FLAG_MISERR FLASH Fast programming data miss error flag
///  *     @arg @ref FLASH_FLAG_FASTERR FLASH Fast programming error flag
//  *     @arg @ref FLASH_FLAG_RDERR FLASH PCROP read  error flag
///  *     @arg @ref FLASH_FLAG_OPTVERR FLASH Option validity error flag
//  *     @arg @ref FLASH_FLAG_ECCC FLASH one ECC error has been detected and corrected
//  *     @arg @ref FLASH_FLAG_ECCD FLASH two ECC errors have been detected
///  *     @arg @ref FLASH_FLAG_ALL_ERRORS FLASH All errors flags
  /* Clear all FLASH flags */
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_WRPERR  | FLASH_FLAG_PGAERR  | FLASH_FLAG_SIZERR |
						   FLASH_FLAG_OPTVERR | FLASH_FLAG_PROGERR | FLASH_FLAG_OPERR |
						   FLASH_FLAG_PGSERR  | FLASH_FLAG_MISERR  | FLASH_FLAG_FASTERR |
						   FLASH_FLAG_ALL_ERRORS);

//	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_RDERR);
	/* Unlock the Program memory */
	HAL_FLASH_Lock();
  
}


uint32_t FLASH_If_Erase(uint32_t StartSector)
{
	uint32_t eraseres = 0;
	FLASH_EraseInitTypeDef desc;

	desc.TypeErase = FLASH_TYPEERASE_PAGES;
	desc.Page      = StartSector;

	if(StartSector < USER_FLASH_LAST_PAGE_ADDRESS)
	{
		desc.NbPages = (USER_FLASH_LAST_PAGE_ADDRESS - StartSector) / USER_FLASH_PAGE_SIZE;
		if(HAL_FLASHEx_Erase(&desc, &eraseres) != HAL_OK)
		{
			eraseres = 0;
		}
	}

	return eraseres;
}



uint32_t FLASH_If_Write(__IO uint32_t* FlashAddress, uint32_t* Data ,uint16_t DataLength)
{
  uint32_t i = 0;
  HAL_StatusTypeDef status = HAL_ERROR;
  
  for (i = 0; (i < DataLength) && (*FlashAddress <= (USER_FLASH_END_ADDRESS-8)); i++)
  {
//    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, *FlashAddress, *(uint32_t*)(Data+i));
	status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, *FlashAddress, *(uint64_t*)(Data+i));
    
    if(HAL_OK == status)
    {
      if (*(uint64_t*)*FlashAddress != *(uint64_t*)(Data+i))
      {
        return 2;
      }
      
      *FlashAddress += 8;
    }
    else
    {
      return 1;
    }
  }
  
  return 0;
}




