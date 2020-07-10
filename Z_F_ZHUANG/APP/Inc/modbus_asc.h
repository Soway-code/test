#ifndef __MODBUS_ASC_H
#define __MODBUS_ASC_H

#include "stm32g0xx_hal.h"

#define MB_ADDRESS_BROADCAST            0x00
#define AD_BUF_MAX             10                                   

#define MB_EX_NONE 			(0x00)
#define MB_EX_ILLEGAL_FUNCTION 		(0x01)                                  //非法功能码
#define MB_EX_ILLEGAL_DATA_ADDRESS	(0x02)                                  //非法数据地址
#define MB_EX_ILLEGAL_DATA_VALUE	(0x03)                                  //非法数据值
#define MB_EX_SLAVE_DEVICE_FAILURE	(0x04)                                  //Flash未使能
#define MB_EX_MEMORY_PARITY_ERROR	(0x05)                                  //校验错误



/*03功能码的地址范围*/
#define MBASC_HOLDING_REG_REGION_BGEIN		        0x30
#define MBASC_HOLDING_REG_REGION_END			0x4F


/*04功能码的地址范围*/
#define MBASC_INPUT_REG_REGION_BGEIN        	        0x00
#define MBASC_INPUT_REG_REGION_END          	        0x0E


/*05功能码的地址范围*/
#define MBASC_SINGLE_COIL_ADDR_BGEIN                    0x50
#define MBASC_SINGLE_COIL_ADDR_END                      0x54


/*10功能码的地址范围*/
#define MBASC_MUL_REG_REGION_BGEIN          MBASC_HOLDING_REG_REGION_BGEIN
#define MBASC_MUL_REG_REGION_END            MBASC_HOLDING_REG_REGION_END


#define ASCII_SINGLE_COIL_ADDR_BGEIN          		0x50//单个寄存器起始地址
#define ASCII_SINGLE_COIL_ADDR_END            		0x53//单个寄存器结束地址

void MBASC_Function(void);
void MBASC_AutoUpLoadFrame(void);
void MBASC_SendMsg(uint8_t *u8Msg, uint8_t u8MsgLen);


#endif




