#ifndef _FLASH_H
#define _FLASH_H

#include "main.h"

#define FLASH_LAST_PAGE_ADDR		              0x0800F800					// Flash最后一个扇区首地址
//#define PARA_START_ADDR				          0x0800FF00					// 参数保存默认起始地址
//#define PARA_WRITE_FLAG_ADDR		              PARA_START_ADDR

#define STANDBY_OFFSET_ADDR					    0x00000800							// 备份参数偏移地址(相对FLASH_BASE_ADDR)
#define STANDBY_BASE_ADDR					      0x08003000							// 参数备份基地址

#define FLASH_BASE_ADDR						0x08003800							// Flash存储参数的基地址
#define COMMON_OFFSET						0x00000000							// 通用参数存储地址偏移量
#define COMMON_BASE_ADDR					(FLASH_BASE_ADDR + COMMON_OFFSET)	// FLASH参数开始地址
#define COMMON_UPGRADE_ADDR					(COMMON_BASE_ADDR + 0)				// 通用参数系统升级标志地址
#define COMMON_SLAVE_ADDR					(COMMON_BASE_ADDR + 1)				// 通用参数从机地址地址
#define COMMON_BAUD_ADDR					(COMMON_BASE_ADDR + 2)				// 通用参数波特率地址
#define COMMON_PARITY_ADDR					(COMMON_BASE_ADDR + 3)				// 通用参数奇偶校验地址
#define COMMON_OUTPUTMOD_ADDR				(COMMON_BASE_ADDR + 4)				// 通用参数输出模式地址
#define COMMON_RESERVED_ADDR				(COMMON_BASE_ADDR + 5)				// 通用参数保留地址

#define CUSTOMER_OFFSET						0x00000010							// 客户参数存储地址偏移量
#define CUSTOMER_BASE_ADDR					(FLASH_BASE_ADDR + CUSTOMER_OFFSET)	// 客户参数基地址
#define	CUSTOMER_FLAG_ADDR					(CUSTOMER_BASE_ADDR + 0)			// 客户参数标志	地址
#define CUSTOMER_POSH_ADDR					(CUSTOMER_BASE_ADDR + 2)			// 客户参数位移高16位地址
#define CUSTOMER_POSL_ADDR					(CUSTOMER_BASE_ADDR + 4)			// 客户参数位移低16位地址
#define CUSTOMER_TEMPH_ADDR					(CUSTOMER_BASE_ADDR + 6)			// 客户参数温度高16位地址
#define CUSTOMER_TEMPL_ADDR					(CUSTOMER_BASE_ADDR + 8)			// 客户参数温度低16位地址
#define CUSTOMER_SLAVE_ADDR					(CUSTOMER_BASE_ADDR + 10)			// 客户参数从机地址
#define CUSTOMER_BAUD_ADDR					(CUSTOMER_BASE_ADDR + 12)			// 客户参数波特率地址
#define CUSTOMER_PARITY_ADDR				(CUSTOMER_BASE_ADDR + 14)			// 客户参数奇偶校验地址
#define CUSTOMER_COMPENSATION_ADDR			(CUSTOMER_BASE_ADDR + 16)			// 客户参数补偿使能地址
#define CUSTOMER_FREQUENCY_ADDR				(CUSTOMER_BASE_ADDR + 18)			// 客户参数数据上传间隔
#define CUSTOMER_UPGRADE_ADDR				(CUSTOMER_BASE_ADDR + 20)			// 客户参数升级标志地址
#define CUSTOMER_SETLOGICALZERO_ADDR		(CUSTOMER_BASE_ADDR + 22)			// 客户参数设置位移逻辑零点地址
#define CUSTOMER_RESET_ADDR					(CUSTOMER_BASE_ADDR + 24)			// 客户参数恢复出厂设置地址
#define CUSTOMER_OUTPUTMOD_ADDR				(CUSTOMER_BASE_ADDR + 26)			// 客户参数输出模式地址
#define CUSTOMER_RESERVED_ADDR				(CUSTOMER_BASE_ADDR + 28)			// 客户参数保留地址

#define EN_INT                  	__enable_irq();				// 系统开全局中断  
#define DIS_INT                 	__disable_irq();			// 系统关全局中断

HAL_StatusTypeDef FlashLastPageErase(void);						// 擦除Flash最后一个扇区
HAL_StatusTypeDef FlashPageErase(uint32_t Address);
uint32_t FlashReadWord(uint32_t faddr);							// 读一个字
uint16_t FlashReadHalfWord(uint32_t faddr);						// 读半字
void FlashReadNHalfWord(uint32_t ReadAddr, uint16_t *pBuffer, uint16_t NumToRead);	// 读取N个半字
void FlashReadNWord(uint32_t ReadAddr, uint32_t *pBuffer, uint16_t NumToRead);		// 读取N个字
void FlashWriteHalfWord(uint32_t Address, uint16_t Data);							// 写半字
void FlashWriteDoubleWord(uint32_t Address, uint64_t *Data, uint16_t DataLen);		// 写N个双字
HAL_StatusTypeDef flash_write(uint32_t address, uint64_t data);
void flash_page_erase(uint32_t page);
uint32_t GetPage(uint32_t Addr);
#endif
