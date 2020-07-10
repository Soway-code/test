#include "inflash.h"

extern FLASH_ProcessTypeDef pFlash;
FLASH_EraseInitTypeDef EraseInitStruct = {0};
uint32_t GetPage(uint32_t Addr);
uint32_t FirstPage = 0, NbOfPages = 0;
uint32_t Address = 0, PageError = 0;
#define FLASH_USER_END_ADDR     (FLASH_BASE + FLASH_SIZE - 1)   /* End @ of user Flash area */
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
    status = HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
	HAL_FLASH_Lock();

	
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


//Flash дN���ֽ�
void FlashWriteNbyte(uint32_t Address,  uint8_t *pBuffer, uint16_t pLen)
{
	uint16_t i;
	
	HAL_FLASH_Unlock();
	for(i=0; i<pLen; i++)
	{
		Address = *pBuffer++;
		Address++;
	}
	HAL_FLASH_Lock();
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
//	DIS_INT;
	
	SET_BIT(FLASH->CR, FLASH_CR_PG);
	*(__IO uint16_t*)Address = Data;

	HAL_FLASH_Lock();

//	EN_INT;
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
uint8_t FlashWriteDoubleWord(uint32_t Address, uint64_t *Data, uint16_t DataLen)		// дN��˫��
{
	
	int8_t status = 0;
	uint16_t i = 0;
	
//	if((Address > FLASH_END) || (Data == NULL) || (DataLen == 0))
//	{
//		status = -1;
//	}

	status = FlashPageErase(Address);
	if(status != HAL_OK)
	{
		status = -1;
	}
	
    HAL_FLASH_Unlock();
	for(i = 0; i < DataLen; i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (Address + (i * 8) + COMMON_BASE_ADDR), Data[i]);
	
	}
	HAL_FLASH_Lock();
	
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




