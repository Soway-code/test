#ifndef _FLASH_H
#define _FLASH_H

#include "main.h"

#define FLASH_LAST_PAGE_ADDR		              0x0800F800					// Flash���һ�������׵�ַ
#define APPLICATION_ADDRESS                       (uint32_t)0x08004000      /* Start user code address: ADDR_FLASH_PAGE_8 */

#define USER_FLASH_PAGE_SIZE          FLASH_PAGE_SIZE
//#define USER_FLASH_END_ADDRESS        0x08020000
//#define USER_FLASH_LAST_PAGE_ADDRESS  0x0801FC00

#define STANDBY_OFFSET_ADDR					 0x00000800							// ���ݲ���ƫ�Ƶ�ַ(���FLASH_BASE_ADDR)
#define STANDBY_BASE_ADDR					 0x08003000							// �������ݻ���ַ

#define FLASH_BASE_ADDR						 0x08003800					        // Flash�洢�����Ļ���ַ
#define COMMON_OFFSET						 0x00000000							// ͨ�ò����洢��ַƫ����
#define COMMON_BASE_ADDR					(FLASH_BASE_ADDR + COMMON_OFFSET)	// FLASH������ʼ��ַ
#define COMMON_UPGRADE_ADDR					(COMMON_BASE_ADDR + 0)				// ͨ�ò���ϵͳ������־��ַ
#define COMMON_SLAVE_ADDR					(COMMON_BASE_ADDR + 1)				// ͨ�ò����ӻ���ַ��ַ
#define COMMON_BAUD_ADDR					(COMMON_BASE_ADDR + 2)				// ͨ�ò��������ʵ�ַ
#define COMMON_PARITY_ADDR					(COMMON_BASE_ADDR + 3)				// ͨ�ò�����żУ���ַ
#define COMMON_OUTPUTMOD_ADDR				(COMMON_BASE_ADDR + 4)				// ͨ�ò������ģʽ��ַ
#define COMMON_RESERVED_ADDR				(COMMON_BASE_ADDR + 5)				// ͨ�ò���������ַ

// ����ʹ��
#define UPGRADE_DISABLE							0x00							// ������־ʧ��(Ĭ��ֵ)
#define UPGRADE_ENABLE							0xFF							// ������־ʹ��

#define EN_INT                  	__enable_irq();				// ϵͳ��ȫ���ж�  
#define DIS_INT                 	__disable_irq();			// ϵͳ��ȫ���ж�

HAL_StatusTypeDef FlashLastPageErase(void);						// ����Flash���һ������
HAL_StatusTypeDef FlashPageErase(uint32_t Address);
uint32_t FlashReadWord(uint32_t faddr);							// ��һ����
uint16_t FlashReadHalfWord(uint32_t faddr);						// ������
void FlashReadNHalfWord(uint32_t ReadAddr, uint16_t *pBuffer, uint16_t NumToRead);	// ��ȡN������
void FlashReadNWord(uint32_t ReadAddr, uint32_t *pBuffer, uint16_t NumToRead);		// ��ȡN����
void FlashWriteHalfWord(uint32_t Address, uint16_t Data);							// д����
uint8_t FlashWriteDoubleWord(uint32_t Address, uint64_t *Data, uint16_t DataLen);		// дN��˫��
uint32_t FLASH_If_Erase(uint32_t StartSector);
uint32_t GetPage(uint32_t Addr);
#endif
