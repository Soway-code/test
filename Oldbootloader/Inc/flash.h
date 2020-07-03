#ifndef _FLASH_H
#define _FLASH_H

#include "main.h"

#define FLASH_LAST_PAGE_ADDR		              0x0800F800					// Flash最后一个扇区首地址
#define APPLICATION_ADDRESS                       (uint32_t)0x08004000      /* Start user code address: ADDR_FLASH_PAGE_8 */

#define USER_FLASH_PAGE_SIZE          FLASH_PAGE_SIZE
//#define USER_FLASH_END_ADDRESS        0x08020000
//#define USER_FLASH_LAST_PAGE_ADDRESS  0x0801FC00

#define STANDBY_OFFSET_ADDR					 0x00000800							// 备份参数偏移地址(相对FLASH_BASE_ADDR)
#define STANDBY_BASE_ADDR					 0x08003000							// 参数备份基地址

#define FLASH_BASE_ADDR						 0x08003800					        // Flash存储参数的基地址
#define COMMON_OFFSET						 0x00000000							// 通用参数存储地址偏移量
#define COMMON_BASE_ADDR					(FLASH_BASE_ADDR + COMMON_OFFSET)	// FLASH参数开始地址
#define COMMON_UPGRADE_ADDR					(COMMON_BASE_ADDR + 0)				// 通用参数系统升级标志地址
#define COMMON_SLAVE_ADDR					(COMMON_BASE_ADDR + 1)				// 通用参数从机地址地址
#define COMMON_BAUD_ADDR					(COMMON_BASE_ADDR + 2)				// 通用参数波特率地址
#define COMMON_PARITY_ADDR					(COMMON_BASE_ADDR + 3)				// 通用参数奇偶校验地址
#define COMMON_OUTPUTMOD_ADDR				(COMMON_BASE_ADDR + 4)				// 通用参数输出模式地址
#define COMMON_RESERVED_ADDR				(COMMON_BASE_ADDR + 5)				// 通用参数保留地址

// 升级使能
#define UPGRADE_DISABLE							0x00							// 升级标志失能(默认值)
#define UPGRADE_ENABLE							0xFF							// 升级标志使能

#define EN_INT                  	__enable_irq();				// 系统开全局中断  
#define DIS_INT                 	__disable_irq();			// 系统关全局中断

HAL_StatusTypeDef FlashLastPageErase(void);						// 擦除Flash最后一个扇区
HAL_StatusTypeDef FlashPageErase(uint32_t Address);
uint32_t FlashReadWord(uint32_t faddr);							// 读一个字
uint16_t FlashReadHalfWord(uint32_t faddr);						// 读半字
void FlashReadNHalfWord(uint32_t ReadAddr, uint16_t *pBuffer, uint16_t NumToRead);	// 读取N个半字
void FlashReadNWord(uint32_t ReadAddr, uint32_t *pBuffer, uint16_t NumToRead);		// 读取N个字
void FlashWriteHalfWord(uint32_t Address, uint16_t Data);							// 写半字
uint8_t FlashWriteDoubleWord(uint32_t Address, uint64_t *Data, uint16_t DataLen);		// 写N个双字
uint32_t FLASH_If_Erase(uint32_t StartSector);
uint32_t GetPage(uint32_t Addr);
#endif
