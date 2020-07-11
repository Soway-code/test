#include "type.h"

uint32_t BaudRateTable[8] = {0, 2400,  4800, 9600, 19200, 38400, 57600, 115200};// 波特率速率表
uint16_t Cycle3T5Byte[8]  = {0, 16042, 8021, 4011,  2006,  1003,   669,    335};// 不同波特率3.5字节对应时长(每字节按11位计算) 单位：us(微秒)
// Cycle3T5Byte 计算公式 T = 3.5 * 11 * 1000000 / Baud (3.5字节*每字节11位*1000000(单位化成微秒)/波特率) 结果向上取整
uint16_t CycleTable[7] = {0, 1000, 500, 200, 100, 50, 20};						// 采样频率对应的周期值 单位：ms(毫秒)

CustomerPara_Typedef Default_CustomerPara = {FLAG_ENABLE,						// 标志位(使能)
  										    0x0000, 0x0C80,					// 数据 高16位在前 低16位在后

											 DEFAULT_ADDR_0x01, 			// 设备地址(0x01)
											 BAUDRATA_9600,						// 波特率(9600)
											 PARITY_NONE,						  // 奇偶校验(无校验)
											 COMPENSATION_DISABLE,		// 补偿使能(失能)
											 FREQUENCY_1S,					// 上传间隔
											 UPGRADE_DISABLE,					// 系统升级标志
											 0x0000,							    // 恢复出厂设置
											 OUTPUTMOD_RTU,						// 输出模式(RTU)
											 0x0000,							    // 保留 方便添加更多参数 
											 0x0000,
                                             0x0000,
                                             0x0000,
                                             0x0000};							    // 保留 方便添加更多参数

											 
											 
		
uint16_t FreTable[8] = {0, 999,1999, 4999, 9999, 29999, 59999,0};						

									 
