#include "flash.h"

extern FLASH_ProcessTypeDef pFlash;
FLASH_EraseInitTypeDef EraseInitStruct = {0};
uint32_t GetPage(uint32_t Addr);
uint32_t FirstPage = 0, NbOfPages = 0;
uint32_t Address = 0, PageError = 0;
//******************************************************************************
// ����               : FlashLastPageErase()
// ��������           : 2019.6.13
// ����               : 
// ����               : ����Flashһ������
// �������           : ��
// �������           : ����״̬��־
// ���ؽ��           : ��
//******************************************************************************
HAL_StatusTypeDef FlashPageErase(uint32_t Address)								// ����Flashһ������
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
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
	
	return status;
}


//******************************************************************************
// ����               : FlashReadNHalfWord()
// ��������           : 2019.6.13
// ����               : 
// ����               : ��ȡFlash N������
// �������           : ReadAddr -- ��ȡFlash�׵�ַ
//						pBuffer -- ��ȡFlash���ݻ����ַ
//						NumToRead -- ��ȡ���ָ���
// �������           : ��
// ���ؽ��           : ��
// ע���˵��         : ��
// �޸�����           : ��
//******************************************************************************
void FlashReadNHalfWord(uint32_t ReadAddr, uint16_t *pBuffer, uint16_t NumToRead)	// ��ȡN������
{
	uint16_t i;
	
    HAL_FLASH_Unlock();
	
	for(i = 0; i < NumToRead; i++)
	{
		pBuffer[i] = *(uint16_t*)(ReadAddr);			// ��ȡ2���ֽ�.
		ReadAddr += 2;									// ƫ��2���ֽ�.	
	}
	
	HAL_FLASH_Lock();
}

//******************************************************************************
// ����               : FlashReadNWord()
// ��������           : 2019.6.13
// ����               : 
// ����               : ��ȡFlash N����
// �������           : ReadAddr -- ��ȡ�����׵�ַ
//						pBuffer -- ��ȡFlash���ݻ����ַ
//						NumToRead -- ��ȡ�ָ���
// �������           : ��
// ���ؽ��           : ��
// ע���˵��         : ��
// �޸�����           : ��
//******************************************************************************
void FlashReadNWord(uint32_t ReadAddr, uint32_t *pBuffer, uint16_t NumToRead)		// ��ȡN����
{
	uint32_t i;
	
    HAL_FLASH_Unlock();
	
	for(i = 0; i < NumToRead; i++)
	{
		pBuffer[i] = FlashReadWord(ReadAddr);			// ��ȡ4���ֽ�.
		ReadAddr += 4;									// ƫ��4���ֽ�.	
	}
	
	HAL_FLASH_Lock();
}

//******************************************************************************
// ����               : FlashReadWord()
// ��������           : 2019.6.13
// ����               : 
// ����               : ��ȡFlash 1����
// �������           : addr -- ��ȡ���ݵ�ַ
// �������           : addr��ַ����
// ���ؽ��           : ��
// ע���˵��         : ��
// �޸�����           : ��
//******************************************************************************
uint32_t FlashReadWord(uint32_t addr)					// ��һ����
{
	return *(uint32_t*)addr; 
}

//******************************************************************************
// ����               : FlashReadHalfWord()
// ��������           : 2019.6.13
// ����               : 
// ����               : ��ȡFlash 1������
// �������           : addr -- ��ȡ���ݵ�ַ
// �������           : addr��ַ����
// ���ؽ��           : ��
// ע���˵��         : ��
// �޸�����           : ��
//******************************************************************************
uint16_t FlashReadHalfWord(uint32_t addr)				// ������
{
	return *(uint16_t*)addr; 
}

//******************************************************************************
// ����               : FlashLastPageErase()
// ��������           : 2019.6.13
// ����               : 
// ����               : ����Flash���һ������
// �������           : ��
// �������           : ����״̬��־
// ���ؽ��           : ��
// ע���˵��         : ��
// �޸�����           : ��
//******************************************************************************
HAL_StatusTypeDef FlashLastPageErase(void)				// ����Flash���һ������
{
	HAL_StatusTypeDef status = HAL_OK;
	FirstPage=GetPage(FLASH_LAST_PAGE_ADDR);
	NbOfPages=1;
	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page        = FirstPage;
    EraseInitStruct.NbPages     = NbOfPages;
	DIS_INT;
	HAL_FLASH_Unlock();

	status = FLASH_WaitForLastOperation(0xFF);			// �ȴ��������
	
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
// ����               : FlashWriteHalfWord()
// ��������           : 2019.6.13
// ����               : 
// ����               : Flash д1������
// �������           : Address -- д���ݵ�ַ
// �������           : ��
// ���ؽ��           : ��
// ע���˵��         : ��
// �޸�����           : ��
//******************************************************************************
void FlashWriteHalfWord(uint32_t Address, uint16_t Data)							// д����
{
	HAL_FLASH_Unlock();
	DIS_INT;
	
	SET_BIT(FLASH->CR, FLASH_CR_PG);
	*(__IO uint16_t*)Address = Data;

	HAL_FLASH_Lock();

	EN_INT;
}


//******************************************************************************
// ����               : FlashWriteNHalfWord()
// ��������           : 2019.6.13
// ����               : 
// ����               : Flash дN������
// �������           : Address -- д�����׵�ַ
//						Data -- д����ָ��
//						DataLen -- д���ݳ���
// �������           : ��
// ���ؽ��           : ��
// ע���˵��         : ��
// �޸�����           : ��
//******************************************************************************
void FlashWriteDoubleWord(uint32_t Address, uint64_t *Data, uint16_t DataLen)		// дN������
{
	uint16_t i =0;
	

	

	for(i = 0; i < DataLen; i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (Address + (i * 8)), Data[i]);
	}

	

}

uint32_t GetPage(uint32_t Addr)
{
  return (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;;
}




