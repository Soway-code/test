
#ifndef _MODBUS_H
#define _MODBUS_H

#include "main.h"

// modbus 功能号
#define MDB_F03_READ_HOLD_REG           0x03                                    // 读保持寄存器            
#define MDB_F04_READ_INPUT_REG          0x04                                    // 读输入寄存器 
#define MDB_F05_EXT_WRITE_COIL_REG      0x05                                    // 写线圈寄存器 标定和清零作用
#define MDB_F10_WRITE_INPUT_REG         0x10                                    // 写保持寄存器 
#define MDB_F25_INT_WRITE_COIL_REG      0x25                                    // 内部写线圈寄存器 标定和清零作用
#define MDB_F26_INT_READ_HOLD_REG     	0x26                                    // 内部读保持寄存器 
#define MDB_F27_INT_WRITE_HOLD_REG		  0x27                                    // 内部写保持寄存器 

// modbus 返回错误码
#define MDB_NO_ERROR                    0x00                                    // 无错误
#define MDB_FNO_INVALID                 0x01                                    // 非支持的功能码
#define MDB_REG_ADDR_INVALID            0x02                                    // 寄存器地址不正确
#define MDB_REG_VALUE_OVERRIDE          0x03                                    // 寄存器值超出范围
#define MDB_MSG_FRAME_CHECK_ERROR		    0x10									                  // CRC/LRC校验错误
#define MDB_MSG_PARITY_ERROR			      0x11									                  // 奇偶校验错误
#define MDB_MSG_LEN_ERROR				        0x12									                  // 帧长度错误
#define MDB_MSG_ADDR_ERROR				      0x13									                  // 地址错误

// modbus 寄存器地址
#define ACC_ADDR_0x02                   0x02                                    // X轴寄存器地址值
#define ACC_ADDR_0x04                   0x04                                    // Y轴寄存器地址值
#define ACC_ADDR_0x06                   0x06                                    // Z轴寄存器地址值
#define SLAVE_ADDR_0x30                 0x30                                    // 从机地址寄存器地址值
#define BAUD_ADDR_0x31                  0x31                                    // 波特率寄存器地址值
#define PARITY_ADDR_0x32                0x32                                    // 奇偶校验寄存器地址值
#define COMPENSATION_ADDR_0x34          0x34                                    // 补偿使能寄存器地址值
#define FREQUENCY_ADDR_0x3F          	  0x3F                                    // 位移和温度采样值更新频率地址值
#define UPGRADE_ADDR_0x40				        0x40									                  // 系统在线升级寄存器地址值
#define	SETLOGICALZERO_ADDR_0x50		    0x50									                  // 设置逻辑零点寄存地址值
#define	RESET_ADDR_0x51					        0x51									                  // 恢复出厂设置寄存地址值
#define OUT_PUT_MOD_ADDR_0x60           0x60                                    // 输出寄存器地址值
#define PosRS_ADDR_0x88					        0x88									                  // 位移标定/清零寄存器地址值
#define PosRangeRS_ADDR_0x89			      0x89									                  // 设定位移零点/满量程
#define TempRS_ADDR_0x9A				        0x9A									                  // 温度标定/清零寄存器地址值
#define TempRangeRS_ADDR_0x9B			      0x9B									                  // 设定温度零点/满量程
#define CompenRS_ADDR_0xCD				      0xCD									                  // 补偿标定清除

// modbus F26功能码读地址标志
#define ReadPos							0x01									// 读位移参数标志
#define ReadTemp						0x02									// 读温度参数标志
#define ReadCompen						0x03									// 读补偿参数标志
#define ReadTestData					0x04									// 读补内测数据标志

// modbus F26功能码各地址段起始和结束地址值
#define PosStartAddr					0x80									// modbus F26功能码位移参数起始地址
#define PosEndAddr						0x87									// modbus F26功能码位移参数结束地址
#define TempStartAddr					0x90									// modbus F26功能码位移温度起始地址
#define TempEndAddr						0x99									// modbus F26功能码位移温度结束地址
#define CompenStartAddr					0xA0									// modbus F26功能码位移补偿起始地址
#define CompenEndAddr					0xCD									// modbus F26功能码位移补偿结束地址
#define	TestDataAddr					0xF0									// modbus F26功能码内测数据开始地址 内测数据寄存个数为6 读取时必须一次读6个数据 

// modbus F27功能码写地址标志
#define WriteTsc						0x01									// 读位移参数标志
#define WriteTemp						0x02									// 读温度参数标志
#define WriteCompen						0x03									// 读补偿参数标志

// modbus 写标记
#define MDBWriteNone					0x0000									// 无任何写操作标记
#define MDBWriteBaudOrParity			0x0001									// 写波特率或奇偶校验标记
#define MDBWriteOutPutMod				0x0002									// 写输出模式标记
#define MDBWriteFrequency				0x0003									// 写采样频率标记
#define MDBWriteUpgrade					0x0004									// 写系统升级标记
#define MDBWriteReset					0x0005									// 写恢复出厂设置标记

uint8_t MDB_Get_Slave_Addr(uint8_t *master_request_data);						// 获取从机地址
uint8_t MDB_Function(void);														// 功能操作函数
uint8_t FrameCheck(void);														// 帧校验检测
uint8_t MDB_F03_Broadcast_Read_Slave_Addr(void);								// 广播读地址
uint8_t MDB_F03_Read_Hold_Reg(void);											// 读保持寄存器
uint8_t MDB_F03_Read_Addr_Baud_Parity(void);									// 读从机地址 波特率 奇偶校验单个或多个连续数据
uint8_t MDB_F03_Read_Single_Hold_Reg(void);										// 读单个保持寄存器
uint8_t MDB_F04_Read_Input_Reg(void);											// 读输入寄存器 
uint8_t MDB_F05_Write_Coil_Reg(void);											// 写线圈 标定功能
uint8_t MDB_F05_Write_SetLogicalZero(void);										// 写逻辑零点线圈
uint8_t MDB_F05_Write_Reset(void);												// 数据备份/恢复出厂设置
uint8_t MDB_F10_Write_Hold_Reg(void);											// 写保持寄存器
uint8_t MDB_F10_Write_Addr_Baud_Parity(void);									// 写从机地址 波特率 奇偶校验单个或多个连续数据
uint8_t MDB_F10_Write_Single_Hold_Reg(void);									// 写单个保持寄存器
uint8_t MDB_F25_Int_Write_Coil_Reg(void);										// 内部写线圈 标定功能
uint8_t MDB_F25_Int_Write_Upgrade(void);										// 写在线升级标志线圈
uint8_t MDB_F25_Int_Write_PosRS(void);											// 内部位移标定清零/失能/使能
uint8_t MDB_F25_Int_Write_PosRangeRS(void);										// 内部标定位移零点和满量程
uint8_t MDB_F25_Int_Write_TempRS(void);											// 内部温度标定使能/清零
uint8_t MDB_F25_Int_Write_TempRangeRS(void);									// 内部标定温度零点和满量程
uint8_t MDB_F25_Int_Write_CompenRS(void);										// 补偿标定清除
uint8_t MDB_F26_Int_Read_Hold_Reg(void);										// 内部读保持寄存器
uint8_t MDB_F26_Int_Read_Pos_Reg(uint16_t StartAddr);							// 内部读位移保持寄存器
uint8_t MDB_F26_Int_Read_Temp_Reg(uint16_t StartAddr);							// 内部读温度保持寄存器
uint8_t MDB_F26_Int_Read_Compen_Reg(uint16_t StartAddr);						// 内部读补偿保持寄存器
uint8_t MDB_F26_Int_Read_Test_Data(uint16_t StartAddr);							// 内部读测试数据
uint8_t MDB_F27_Int_Write_Hold_Reg(void);										// 内部写保持寄存器
uint8_t MDB_F27_Int_Write_Pos_Reg(uint16_t StartAddr);							// 内部写位移保持寄存器
uint8_t MDB_F27_Int_Write_Temp_Reg(uint16_t StartAddr);							// 内部写温度保持寄存器
uint8_t MDB_F27_Int_Write_Compen_Reg(uint16_t StartAddr);						// 内部写补偿保持寄存器
void WriteCurParaToFlash(void);													// 把当前参数写入Flash 备份区不更新
void BackupCurParaToFlash(void);												// 把当前参数保存到当前参数区和备份区
void MDB_Snd_Msg(uint8_t *pMsg, uint8_t len);									// 发送消息
void WriteParaInit(void);														// Flash写入新的参数时 部分需要对外设重新初始化
#endif

