#ifndef _FLASH_H
#define _FLASH_H

#include "main.h"

#define FLASH_LAST_PAGE_ADDR		              0x0800F800					// Flash���һ�������׵�ַ
//#define PARA_START_ADDR				          0x0800FF00					// ��������Ĭ����ʼ��ַ
//#define PARA_WRITE_FLAG_ADDR		              PARA_START_ADDR

#define STANDBY_OFFSET_ADDR					    0x00000800							// ���ݲ���ƫ�Ƶ�ַ(���FLASH_BASE_ADDR)
#define STANDBY_BASE_ADDR					      0x08003000							// �������ݻ���ַ

#define FLASH_BASE_ADDR						0x08003800							// Flash�洢�����Ļ���ַ
#define COMMON_OFFSET						0x00000000							// ͨ�ò����洢��ַƫ����
#define COMMON_BASE_ADDR					(FLASH_BASE_ADDR + COMMON_OFFSET)	// FLASH������ʼ��ַ
#define COMMON_UPGRADE_ADDR					(COMMON_BASE_ADDR + 0)				// ͨ�ò���ϵͳ������־��ַ
#define COMMON_SLAVE_ADDR					(COMMON_BASE_ADDR + 1)				// ͨ�ò����ӻ���ַ��ַ
#define COMMON_BAUD_ADDR					(COMMON_BASE_ADDR + 2)				// ͨ�ò��������ʵ�ַ
#define COMMON_PARITY_ADDR					(COMMON_BASE_ADDR + 3)				// ͨ�ò�����żУ���ַ
#define COMMON_OUTPUTMOD_ADDR				(COMMON_BASE_ADDR + 4)				// ͨ�ò������ģʽ��ַ
#define COMMON_RESERVED_ADDR				(COMMON_BASE_ADDR + 5)				// ͨ�ò���������ַ

#define CUSTOMER_OFFSET						0x00000010							// �ͻ������洢��ַƫ����
#define CUSTOMER_BASE_ADDR					(FLASH_BASE_ADDR + CUSTOMER_OFFSET)	// �ͻ���������ַ
#define	CUSTOMER_FLAG_ADDR					(CUSTOMER_BASE_ADDR + 0)			// �ͻ�������־	��ַ
#define CUSTOMER_POSH_ADDR					(CUSTOMER_BASE_ADDR + 2)			// �ͻ�����λ�Ƹ�16λ��ַ
#define CUSTOMER_POSL_ADDR					(CUSTOMER_BASE_ADDR + 4)			// �ͻ�����λ�Ƶ�16λ��ַ
#define CUSTOMER_TEMPH_ADDR					(CUSTOMER_BASE_ADDR + 6)			// �ͻ������¶ȸ�16λ��ַ
#define CUSTOMER_TEMPL_ADDR					(CUSTOMER_BASE_ADDR + 8)			// �ͻ������¶ȵ�16λ��ַ
#define CUSTOMER_SLAVE_ADDR					(CUSTOMER_BASE_ADDR + 10)			// �ͻ������ӻ���ַ
#define CUSTOMER_BAUD_ADDR					(CUSTOMER_BASE_ADDR + 12)			// �ͻ����������ʵ�ַ
#define CUSTOMER_PARITY_ADDR				(CUSTOMER_BASE_ADDR + 14)			// �ͻ�������żУ���ַ
#define CUSTOMER_COMPENSATION_ADDR			(CUSTOMER_BASE_ADDR + 16)			// �ͻ���������ʹ�ܵ�ַ
#define CUSTOMER_FREQUENCY_ADDR				(CUSTOMER_BASE_ADDR + 18)			// �ͻ����������ϴ����
#define CUSTOMER_UPGRADE_ADDR				(CUSTOMER_BASE_ADDR + 20)			// �ͻ�����������־��ַ
#define CUSTOMER_SETLOGICALZERO_ADDR		(CUSTOMER_BASE_ADDR + 22)			// �ͻ���������λ���߼�����ַ
#define CUSTOMER_RESET_ADDR					(CUSTOMER_BASE_ADDR + 24)			// �ͻ������ָ��������õ�ַ
#define CUSTOMER_OUTPUTMOD_ADDR				(CUSTOMER_BASE_ADDR + 26)			// �ͻ��������ģʽ��ַ
#define CUSTOMER_RESERVED_ADDR				(CUSTOMER_BASE_ADDR + 28)			// �ͻ�����������ַ

#define EN_INT                  	__enable_irq();				// ϵͳ��ȫ���ж�  
#define DIS_INT                 	__disable_irq();			// ϵͳ��ȫ���ж�

HAL_StatusTypeDef FlashLastPageErase(void);						// ����Flash���һ������
HAL_StatusTypeDef FlashPageErase(uint32_t Address);
uint32_t FlashReadWord(uint32_t faddr);							// ��һ����
uint16_t FlashReadHalfWord(uint32_t faddr);						// ������
void FlashReadNHalfWord(uint32_t ReadAddr, uint16_t *pBuffer, uint16_t NumToRead);	// ��ȡN������
void FlashReadNWord(uint32_t ReadAddr, uint32_t *pBuffer, uint16_t NumToRead);		// ��ȡN����
void FlashWriteHalfWord(uint32_t Address, uint16_t Data);							// д����
void FlashWriteDoubleWord(uint32_t Address, uint64_t *Data, uint16_t DataLen);		// дN��˫��
HAL_StatusTypeDef flash_write(uint32_t address, uint64_t data);
void flash_page_erase(uint32_t page);
uint32_t GetPage(uint32_t Addr);
#endif
