#ifndef _TYPE_H
#define _TYPE_H

#include "main.h"
// ADC
#define SamplingArrayLen						(uint16_t)1000					// 位移和温度采样数组长度
// Flash写标志位
#define FLAG_ENABLE								0x5555							// 标志使能

// 从机地址
#define BROADCAST_ADDR_0x00         0x0000							// 广播地址
#define DEFAULT_ADDR_0x01						0x0001							// 从机地址(默认值)

// 补偿使能
#define COMPENSATION_ENABLE						0x0001							// 补偿使能
#define COMPENSATION_DISABLE					0x0002							// 补偿失能(默认值)

// 波特率参数
#define BAUDRATA_2400							  0x0001							// 波特率2400
#define BAUDRATA_4800							  0x0002							// 波特率4800
#define BAUDRATA_9600							  0x0003							// 波特率9600(默认值)
#define BAUDRATA_19200							0x0004							// 波特率19200
#define BAUDRATA_38400							0x0005							// 波特率38400
#define BAUDRATA_57600							0x0006							// 波特率57600
#define BAUDRATA_115200							0x0007							// 波特率115200

// 奇偶校验参数
#define PARITY_ODD								0x0001							// 奇校验
#define PARITY_EVEN								0x0002							// 偶校验(默认值)
#define PARITY_NONE								0x0003							// 无校验

// 上传频率

#define FREQUENCY_1S							0x0001							// 间隔1S
#define FREQUENCY_2S							0x0002							// 间隔2S
#define FREQUENCY_5S							0x0003							// 间隔5S
#define FREQUENCY_10S							0x0004							// 间隔10S
#define FREQUENCY_30S							0x0005							// 间隔30S
#define FREQUENCY_60S							0x0006							// 间隔60S
#define FREQUENCY_Stop							0x0007							// 停止上传

// 输出模式
#define OUTPUTMOD_RTU							  0x0001							// RTU模式(默认值)
#define OUTPUTMOD_ASCII							0x0002							// ASCII模式

// 升级使能
#define UPGRADE_DISABLE							0x00							// 升级标志失能(默认值)
#define UPGRADE_ENABLE							0xFF							// 升级标志使能

#define SamplingUnfinish						0x00							// 采样未完成标记
#define SamplingFinish							0x01							// 采样完成标记


extern uint16_t FreTable[8];


typedef struct
{
	uint8_t UpgradeFlag;														// 系统升级标志 存储物理地址 Addr_0x08003000 
	uint8_t Addr;																    // 从机ID 		存储物理地址 Addr_0x08003001
	uint8_t Baud;																    // 波特率		存储物理地址 Addr_0x08003002
	uint8_t Parity;																  // 奇偶校验 	存储物理地址 Addr_0x08003003
	uint8_t OutPutMod;															// 输出模式 	存储物理地址 Addr_0x08003004
	uint8_t Reserved[11];														// 保留 留做备用 方便添加更多参数 按16字节对齐
}CommonPara_Typedef;															// 通用参数结构体 通用参数与客户可操作参数结构体部分参数重叠 仅存储格式不同
																				          // 目的是为了兼容BootLoad和Modbus等定义
typedef struct
{
  uint16_t Flag;																  // Flash写标志
  uint16_t TempH;                             	  // Addr_0x02 数据高16位 
  uint16_t TempL;                             	  // Addr_0x03 数据低16位
  uint16_t Addr;                             		  // Addr_0x30 设备地址
	uint16_t Baud;                             		  // Addr_0x31 波特率
	uint16_t Parity;                            	  // Addr_0x32 奇偶校验
	uint16_t Compensation;                      	  // Addr_0x34 补偿使能
	uint16_t UploadFreq;                      		  // Addr_0x3F 自动上传时间
  uint16_t UpgradeFlag;								// Addr_0x40 系统升级标志
  uint16_t Reset;                             	  // Addr_0x51 恢复出厂设置
  uint16_t OutPutMod;                         	  // Addr_0x60 输出模式
	uint16_t Reserved[5];													  // 保留 留做备用 方便添加更多参数 按16字对齐
}CustomerPara_Typedef;													  // 客户可操作参数结构体



typedef struct
{
	CommonPara_Typedef		Common;												// 通用参数结构体 通用参数与客户可操作参数结构体部分参数重叠 仅存储格式不同 为兼容BootLoad和Modbus规范 
	CustomerPara_Typedef	Customer;											// 客户可操作参数结构体 

}FlashPara_Typedef;																// Flash保存参数结构体 结构体中的Data数组用作对齐

#endif


