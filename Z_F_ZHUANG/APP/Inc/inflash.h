#ifndef _INFLASH_H
#define _INFLASH_H

#include "stm32g0xx_hal.h"

#define FLASH_BASE_ADDRESS           					0x0801E800                          ///< ����Flash����ַ

#define STANDBY_OFFSET_ADDR					    		0x00000800								// ���ݲ���ƫ�Ƶ�ַ(���FLASH_BASE_ADDR)
#define STANDBY_BASE_ADDR					      		0x08003000								// �������ݻ���ַ

#define FLASH_BASE_ADDR						      		0x08003800								// Flash�洢�����Ļ���ַ
#define COMMON_OFFSET									0x00000000								// ͨ�ò����洢��ַƫ����
#define COMMON_BASE_ADDR								(FLASH_BASE_ADDR + COMMON_OFFSET)
#define COMMON_UPGRADE_ADDR								(COMMON_BASE_ADDR + 0)					// ͨ�ò���ϵͳ������־��ַ
#define COMMON_SLAVE_ADDR								(COMMON_BASE_ADDR + 1)					// ͨ�ò����ӻ���ַ��ַ
#define COMMON_BAUD_ADDR								(COMMON_BASE_ADDR + 2)					// ͨ�ò��������ʵ�ַ
#define COMMON_PARITY_ADDR								(COMMON_BASE_ADDR + 3)					// ͨ�ò�����żУ���ַ
#define COMMON_OUTPUTMOD_ADDR							(COMMON_BASE_ADDR + 4)					// ͨ�ò������ģʽ��ַ
#define COMMON_RESERVED_ADDR							(COMMON_BASE_ADDR + 5)					// ͨ�ò���������ַ


#define RUN_ADDR_BASE				                    0x00
//������ʼ����־λ������ÿ���ϵ綼дEEPROMP
//#define SLAVE_ADDR					                    (RUN_ADDR_BASE		 + 0x01)            //�ӻ���ַ
#define SLAVE_ADDR					                    ( 0x01)            	                    //�ӻ���ַ
#define BAUDRATE		                                (SLAVE_ADDR	         + 0x01)            //������
#define PARITY			                                (BAUDRATE	         + 0x01)            //��żУ��λ

#define WORK_TIME_BASE                                  (PARITY + 0x01)                         //������ʱ���ֵ
#define STOP_TIME_BASE                                  (WORK_TIME_BASE + 0x04)                 //ֹͣʱ���ֵ
#define POSITIVE_ROTATE_TIME_BASE                       (STOP_TIME_BASE + 0x04)                 //��תʱ���ֵ
#define NEGATIVE_ROTATE_TIME_BASE                       (POSITIVE_ROTATE_TIME_BASE + 0x04)      //��תʱ���ֵ
#define PULSE_TOTAL_BASE                                (NEGATIVE_ROTATE_TIME_BASE + 0x04)      //��������ֵ
#define DURATION_BASE                                   (PULSE_TOTAL_BASE + 0x04)               //��������ֵ

#define USER_DEFAULT_LEN								(DURATION_BASE + 0x04)


//---------------------------------------------------
#define COMPANY											(USER_DEFAULT_LEN + 0x40) 
#define DEV_ENCODING				                    (COMPANY + 0x40)
#define HWVERSION										(DEV_ENCODING + 0x40)
#define SFVERSION										(HWVERSION + 0x40)
#define DEV_ID											(SFVERSION + 0x40)
#define CUSTOMERCODE                                    (DEV_ID + 0x40)  



HAL_StatusTypeDef FlashLastPageErase(void);						// ����Flash���һ������
HAL_StatusTypeDef FlashPageErase(uint32_t Address);
uint32_t FlashReadWord(uint32_t faddr);							// ��һ����
uint16_t FlashReadHalfWord(uint32_t faddr);						// ������
void FlashReadNHalfWord(uint32_t ReadAddr, uint16_t *pBuffer, uint16_t NumToRead);	// ��ȡN������
void FlashReadNWord(uint32_t ReadAddr, uint32_t *pBuffer, uint16_t NumToRead);		// ��ȡN����
void FlashWriteNbyte(uint32_t Address,  uint8_t *pBuffer, uint16_t pLen);
void FlashWriteHalfWord(uint32_t Address, uint16_t Data);							// д����
uint8_t FlashWriteDoubleWord(uint32_t Address, uint64_t *Data, uint16_t DataLen);		// дN��˫��
uint32_t FLASH_If_Erase(uint32_t StartSector);
uint32_t GetPage(uint32_t Addr);

#endif




