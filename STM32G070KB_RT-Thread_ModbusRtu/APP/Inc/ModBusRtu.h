/**@file        ModbusRtu.h
* @details      ModbusRtu.c的头文件,声明了Modbus RTU 通信的API函数
* @author       杨春林
* @date         2020-04-29
* @version      V1.0.0
* @copyright    2020-2030,深圳市信为科技发展有限公司
**********************************************************************************
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/04/29  <td>1.0.0    <td>杨春林    <td>创建初始版本
* </table>
*
**********************************************************************************
*/

#ifndef __MODBUSRTU_H
#define __MODBUSRTU_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "usart_app.h"
#include "ModBus_Conf.h"


#define SEND_SIZE                       32              ///< 发送缓存大小

#ifndef RECEIVE_SIZE
#define RECEIVE_SIZE                    32              ///< 接收缓存的大小
#endif

#define AUTOUPLOAD_CYCLE                1000            ///< 自动上传周期,单位 ms

#define BROADCASTADDR                   0x00                                  ///< 广播地址

#define HOLDING_REG_REGION1_BGEIN       0x0030                                ///< 保持寄存器起始地址1
#define HOLDING_REG_REGION1_END         0x0041                                ///< 保持寄存器结束地址1

#define INPUT_REG_REGION1_BEGIN         0x0000                                ///< 输入寄存器起始地址1
#define INPUT_REG_REGION1_END           0x0006                                ///< 输入寄存器结束地址1
#define INPUT_REG_REGION2_BEGIN         0x0080                                ///< 输入寄存器起始地址2
#define INPUT_REG_REGION2_END           0x0080                                ///< 输入寄存器结束地址2

#define SINGLE_COIL_REGION1_BEGIN       0x0050                                ///< 单个线圈寄存器起始地址1
#define SINGLE_COIL_REGION1_END         0x0053                                ///< 单个线圈寄存器结束地址1

#define SPECIAL_COIL_REGION1_BEGIN      0x0000                                ///< 特殊寄存器起始地址1
#define SPECIAL_COIL_REGION1_END        0x0005                                ///< 特殊寄存器结束地址1

#define MUL_REG_REGION1_BEGIN           0x0030                                ///< 多个寄存器起始地址1
#define MUL_REG_REGION1_END             0x0041                                ///< 多个寄存器结束地址1

#define _25_FNUC_REG_REGION1_BEGIN      0x0000                                ///< 自定义25功能码寄存器起始地址
#define _25_FNUC_REG_REGION1_END        0x0009                                ///< 自定义25功能码寄存器结束地址

#define DEF_MUL_REG_REGION1_BEGIN       0x0080                                ///< 自定义26/27功能码寄存器起始地址1
#define DEF_MUL_REG_REGION1_END         0x00A2                                ///< 自定义26/27功能码寄存器结束地址1

#define MUL_VERSION_INF_BEGIN           0x00E0                                ///< 版本信息起始地址
#define MUL_VERSION_INF_END             0x00E5                                ///< 版本信息结束地址

#define MB_NONE_EXCEPTION               0x00                                  ///< Modbus无异常
#define MB_FUNC_EXCEPTION               0x01                                  ///< Modbus功能码无效
#define MB_ADDR_EXCEPTION               0x02                                  ///< Modbus访问地址失败
#define MB_VALU_EXCEPTION               0x03                                  ///< Modbus访问数值无效
#define MB_DEVC_EXCEPTION               0x04                                  ///< Modbus访问设备失败
#define MB_ACKN_EXCEPTION               0x05                                  ///< Modbus访问设备无应答
#define MB_BUSY_EXCEPTION               0x06                                  ///< Modbus访问设备忙
#define MB_PARI_EXCEPTION               0x08                                  ///< Modbus设备校验错误
#define MB_REQ_FAILURE                  0x80                                  ///< Modbus请求错误

#define CALIB_CAPMIN_FLAG               0x01                                  ///< 标定电容零点标志
#define CALIB_CAPMAX_FLAG               0x10                                  ///< 标定电容满度标志
#define CALIB_CAPEOC_FLAG               0x11                                  ///< 标点电容结束标志

#define CALIB_CAPADMIN_FLAG             0x01                                  ///< 标定电容AD零点标志
#define CALIB_CAPADLOW_FLAG             0x02                                  ///< 标定电容AD下刻度标志
#define CALIB_CAPADHIH_FLAG             0x10                                  ///< 标定电容AD上刻度标志
#define CALIB_CAPADMAX_FLAG             0x20                                  ///< 标定电容AD满度标志
#define CALIB_CAPADEOC_FLAG             0x33                                  ///< 标定电容AD结束标志

#define CALIB_CAPDAMIN_FLAG             0x01                                  ///< 标定电容DA零点标志
#define CALIB_CAPDALOW_FLAG             0x02                                  ///< 标定电容DA下刻度标志
#define CALIB_CAPDAHIH_FLAG             0x10                                  ///< 标定电容DA上刻度标志
#define CALIB_CAPDAMAX_FLAG             0x20                                  ///< 标定电容DA满度标志
#define CALIB_CAPDAEOC_FLAG             0x33                                  ///< 标定电容DA结束标志

#define CALIB_TEMPDAMIN_FLAG            0x01                                  ///< 标定温度DA零点标志
#define CALIB_TEMPDAMAX_FLAG            0x10                                  ///< 标定温度DA满度标志
#define CALIB_TEMPDAEOC_FLAG            0x11   

///< 冻结设备
#define FREEZE_ENABLE                   0x0F
///< 解冻设备
#define FREEZE_DISABLE                  0x00

#define ADCLIB_ENABLE                   0x0F
#define ADCLIB_DISABLE                  0x00

#define NO_CHECK_ADDRESS                0
#define CHECK_ADDRESS                   1
    
    
/** ModBus发送/接收处理结构体 */
typedef struct {
    uint8_t *Send_Buf;                  ///< ModBus发送缓存指针
    uint16_t Send_Len;                  ///< ModBus发送缓存长度
    uint8_t *Receive_Buf;               ///< ModBus接收缓存指针
}ModBus_TX_RX_TypeDef;

/** ModBus处理的基本参数结构 */
typedef struct _ModBusBaseParam_TypeDef {
    uint8_t Device_Addr;                ///< 设备地址
    uint8_t BaudRate;                   ///< 串口波特率代码
    uint8_t Parity;                     ///< 串口奇偶校验代码
    uint8_t AutoUpload;                 ///< 自动上传周期
    uint8_t Output_Mode;                ///< 输出模式
    uint8_t Factory_Reset;              ///< 恢复出厂设置
    uint8_t Freeze;                     ///< 设备冻结
    uint8_t InRomWrEn;                  ///< 内部存储器写使能
    ModBus_TX_RX_TypeDef ModBus_TX_RX;  ///< ModBus发送/接收处理结构体
#ifdef USING_RT_THREAD_OS
    rt_sem_t TX_Lock;                   ///< 发送锁
#endif
    /** ModBus回调函数 */
    int     (*ModBus_CallBack) (struct _ModBusBaseParam_TypeDef *arg);
}ModBusBaseParam_TypeDef;


#ifdef __IN_FLASH_APP_H

/**@brief       ModBus初始化
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体
* @return       函数执行结果
* - None
*/
void ModBus_Init(ModBusBaseParam_TypeDef *ModBusBaseParam);
#else

/**@brief       ModBus初始化
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体
* @param[in]    Param : ModBus初始化的数据内容
* @param[in]    Param_Size : Param的数据长度
* @return       函数执行结果
* - OP_SUCCESS(成功)
* - OP_FAILED(失败)
* @note         Param的数据内容(按字节顺序排列): 0,设备地址; 
* 1,波特率代码; 2奇偶校验代码; 3自动上传周期; 4,输出模式;
* 5,设备冻结标志;
*/
uint8_t ModBus_Init(ModBusBaseParam_TypeDef *ModBusBaseParam,
                uint8_t *Param, uint16_t Param_Size);
#endif

/**@brief       对输入的消息帧进行CRC16校验
* @param[in]    Msg : 消息首地址;
* @param[in]    MsgLen : 消息帧长度;
* @return       函数执行结果
* - CRC16校验码
* @note         消息长度不能为0
*/
uint16_t MDB_Get_CRC16(uint8_t *Msg, uint16_t MsgLen);

/**@brief       发送 Modbus RTU消息帧
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;
* @param[in]    Msg : 消息首地址; 
* @param[in]    MsgLen : 消息帧长度;
* @param[in]    Check_Addr : 是否检查设备地址有效
* @return       函数执行结果
* - None
* @note         消息长度不能为0，发送时先解冻设备
*/
void MDB_Snd_Msg_RTU(ModBusBaseParam_TypeDef *ModBusBaseParam, uint8_t *pMsg, uint16_t len, uint8_t Check_Addr);

/**@brief       发送数据
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;
* @param[in]    Msg : 消息首地址;
* @param[in]    MsgLen : 消息帧长度;
* @return       函数执行结果
* - None
*/
void Send_Data(ModBusBaseParam_TypeDef *ModBusBaseParam, uint8_t *pMsg, uint16_t len);

/**@brief       Modbus RTU消息帧处理函数
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;
* @param[in]    arg : 用户自定义的参数,例如设备参数
* @return       函数执行结果
* - None
*/
void ModbusHandle(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg);

/**@brief       Modbus 03功能码消息帧处理
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;
* @param[in]    arg : 用户自定义的参数,例如设备参数
* @return       函数执行结果
* - None
*/
void ModbusFunc03(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg);

/**@brief       Modbus 04功能码消息帧处理
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;
* @param[in]    arg : 用户自定义的参数,例如设备参数
* @return       函数执行结果
* - None
*/
void ModbusFunc04(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg);

/**@brief       Modbus 05功能码消息帧处理
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;
* @param[in]    arg : 用户自定义的参数,例如设备参数
* @return       函数执行结果
* - None
*/
void ModbusFunc05(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg);

/**@brief       Modbus 10功能码消息帧处理
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;
* @param[in]    arg : 用户自定义的参数,例如设备参数
* @return       函数执行结果
* - None
*/
void ModbusFunc10(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg);

/**@brief       Modbus 25功能码消息帧处理
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;
* @param[in]    arg : 用户自定义的参数,例如设备参数
* @return       函数执行结果
* - None
*/
void ModbusFunc25(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg);

/**@brief       Modbus 26功能码消息帧处理
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;
* @param[in]    arg : 用户自定义的参数,例如设备参数
* @return       函数执行结果
* - None
*/
void ModbusFunc26(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg);

/**@brief       Modbus 27功能码消息帧处理
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;
* @param[in]    arg : 用户自定义的参数,例如设备参数
* @return       函数执行结果
* - None
*/
void ModbusFunc27(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg);

/**@brief       Modbus 2A功能码消息帧处理
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;
* @return       函数执行结果
* - None
*/
void ModbusFunc2A(ModBusBaseParam_TypeDef *ModBusBaseParam);

/**@brief       Modbus 2B功能码消息帧处理
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;
* @return       函数执行结果
* - None
*/
void ModbusFunc2B(ModBusBaseParam_TypeDef *ModBusBaseParam);

/**@brief       Modbus 41功能码消息帧处理
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;
* @return       函数执行结果
* - None
*/
void ModbusFunc41(ModBusBaseParam_TypeDef *ModBusBaseParam);

/**@brief       Modbus 消息帧自动上传处理
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;
* @param[in]    arg : 用户自定义的参数,例如设备参数
* @return       函数执行结果
* - None
*/
void ModbusAutoUpload(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg);

/**@brief       传感器事件处理,自动上传周期不为0时,周期性地调用ModbusAutoUpload
* @param[in]    ModBusBaseParam : ModBus处理的基本参数结构体;
* @param[in]    arg : 用户自定义的参数,例如设备参数
* @return       函数执行结果
* - None
*/
void SensorEvent(ModBusBaseParam_TypeDef *ModBusBaseParam, void *arg);


#ifdef __cplusplus
}
#endif
#endif
