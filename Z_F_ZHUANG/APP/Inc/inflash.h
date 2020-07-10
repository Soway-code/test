#ifndef _INFLASH_H
#define _INFLASH_H

#include "stm32g0xx_hal.h"

#define FLASH_BASE_ADDRESS           					0x0801E800                          ///< 操作Flash基地址

#define STANDBY_OFFSET_ADDR					    		0x00000800								// 备份参数偏移地址(相对FLASH_BASE_ADDR)
#define STANDBY_BASE_ADDR					      		0x08003000								// 参数备份基地址

#define FLASH_BASE_ADDR						      		0x08003800								// Flash存储参数的基地址
#define COMMON_OFFSET									0x00000000								// 通用参数存储地址偏移量
#define COMMON_BASE_ADDR								(FLASH_BASE_ADDR + COMMON_OFFSET)
#define COMMON_UPGRADE_ADDR								(COMMON_BASE_ADDR + 0)					// 通用参数系统升级标志地址
#define COMMON_SLAVE_ADDR								(COMMON_BASE_ADDR + 1)					// 通用参数从机地址地址
#define COMMON_BAUD_ADDR								(COMMON_BASE_ADDR + 2)					// 通用参数波特率地址
#define COMMON_PARITY_ADDR								(COMMON_BASE_ADDR + 3)					// 通用参数奇偶校验地址
#define COMMON_OUTPUTMOD_ADDR							(COMMON_BASE_ADDR + 4)					// 通用参数输出模式地址
#define COMMON_RESERVED_ADDR							(COMMON_BASE_ADDR + 5)					// 通用参数保留地址


#define RUN_ADDR_BASE				                    0x00
//参数初始化标志位，避免每次上电都写EEPROMP
//#define SLAVE_ADDR					                    (RUN_ADDR_BASE		 + 0x01)            //从机地址
#define SLAVE_ADDR					                    ( 0x01)            	                    //从机地址
#define BAUDRATE		                                (SLAVE_ADDR	         + 0x01)            //波特率
#define PARITY			                                (BAUDRATE	         + 0x01)            //奇偶校验位

#define WORK_TIME_BASE                                  (PARITY + 0x01)                         //工作总时间基值
#define STOP_TIME_BASE                                  (WORK_TIME_BASE + 0x04)                 //停止时间基值
#define POSITIVE_ROTATE_TIME_BASE                       (STOP_TIME_BASE + 0x04)                 //正转时间基值
#define NEGATIVE_ROTATE_TIME_BASE                       (POSITIVE_ROTATE_TIME_BASE + 0x04)      //反转时间基值
#define PULSE_TOTAL_BASE                                (NEGATIVE_ROTATE_TIME_BASE + 0x04)      //脉冲数基值
#define DURATION_BASE                                   (PULSE_TOTAL_BASE + 0x04)               //脉冲数基值

#define USER_DEFAULT_LEN								(DURATION_BASE + 0x04)


//---------------------------------------------------
#define COMPANY											(USER_DEFAULT_LEN + 0x40) 
#define DEV_ENCODING				                    (COMPANY + 0x40)
#define HWVERSION										(DEV_ENCODING + 0x40)
#define SFVERSION										(HWVERSION + 0x40)
#define DEV_ID											(SFVERSION + 0x40)
#define CUSTOMERCODE                                    (DEV_ID + 0x40)  



HAL_StatusTypeDef FlashLastPageErase(void);						// 擦除Flash最后一个扇区
HAL_StatusTypeDef FlashPageErase(uint32_t Address);
uint32_t FlashReadWord(uint32_t faddr);							// 读一个字
uint16_t FlashReadHalfWord(uint32_t faddr);						// 读半字
void FlashReadNHalfWord(uint32_t ReadAddr, uint16_t *pBuffer, uint16_t NumToRead);	// 读取N个半字
void FlashReadNWord(uint32_t ReadAddr, uint32_t *pBuffer, uint16_t NumToRead);		// 读取N个字
void FlashWriteNbyte(uint32_t Address,  uint8_t *pBuffer, uint16_t pLen);
void FlashWriteHalfWord(uint32_t Address, uint16_t Data);							// 写半字
uint8_t FlashWriteDoubleWord(uint32_t Address, uint64_t *Data, uint16_t DataLen);		// 写N个双字
uint32_t FLASH_If_Erase(uint32_t StartSector);
uint32_t GetPage(uint32_t Addr);

#endif




