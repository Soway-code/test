/**@file        Picocap_app.h
* @details      Picocap_app.c的头文件,定义了PCap应用的宏定义,声明了PCap应用的API函数
* @author       杨春林
* @date         2020-04-30
* @version      V1.0.0
* @copyright    2020-2030,深圳市信为科技发展有限公司
**********************************************************************************
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/04/30  <td>1.0.0    <td>杨春林    <td>创建初始版本
* </table>
*
**********************************************************************************
*/

#ifndef __PICOCAP_APP_H
#define __PICOCAP_APP_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "Picocap.h"
#include "In_Flash_app.h"


#define PCAP_COLLECT_CYCLE              100     ///< PCap采集周期，单位 ms

#define DATA_BUF_MAX                    10      ///< PCap接收原始数据值的缓存上限

#define DATA_2ND_FILTER_BUF_MAX         10      ///< PCap二阶滤波缓存上限

#define DATA_1ST_FILTER_BUF_MAX         96      ///< PCap一阶滤波缓存上限

#define PCAP_DAC_MIN_VALUE              0       ///< PCap DAC输出最小值

#define PCAP_DAC_MAX_VALUE              4095    ///< PCap DAC输出最大值

#define PCAP_ADC_MIN_VALUE              0       ///< PCap ADC输出最小值

#define PCAP_ADC_MAX_VALUE              65535   ///< PCap ADC输出最大值

/** 数据滤波需要的参数结构 */
typedef struct {    
    uint8_t FilterStart;                ///< 滤波开始标志
    uint8_t InputCountMax;              ///< 累计输入的最大值，上限为DATA_BUF_MAX
    uint8_t FilterFactor;               ///< 滤波系数
    uint8_t FilterBufMax;               ///< 滤波最大缓存
    uint16_t FilterCycle;               ///< 滤波周期
    uint32_t InputRangeMax;             ///< 输入范围最大值
    uint32_t InputRangeMin;             ///< 输入范围最小值 
}DataFilterParam;
    
/** PCap做数据转换需要的参数结构 */
typedef struct {
    uint8_t CompenEn;                   ///< 补偿使能
    uint16_t HeightRange;               ///< 高度量程
    uint16_t CapADMin;                  ///< 电容AD值零点
    uint16_t CapADLow;                  ///< 电容AD值下刻度
    uint16_t CapADHigh;                 ///< 电容AD值上刻度
    uint16_t CapADMax;                  ///< 电容AD值满量程
    uint16_t CapDAMin;                  ///< 电容DA值零点
    uint16_t CapDALow;                  ///< 电容DA值下刻度
    uint16_t CapDAHigh;                 ///< 电容DA值上刻度
    uint16_t CapDAMax;                  ///< 电容DA值满量程
    uint32_t CapMin;                    ///< 电容零点
    uint32_t CapMax;                    ///< 电容满量程
    uint32_t CapMinBak;                 ///< 电容零点
    uint32_t CapMaxBak;                 ///< 电容满量程
    uint8_t CapDA_ClibEn;               ///< 电容DA标定使能
    float Correct_K;                    ///< 电容修正系数K
    float Correct_B;                    ///< 电容修正系数B
}PCap_DataConvert_Param;

/** PCap转换后输出数据的结构 */
typedef struct {
    uint16_t LiquidHeightAD;            ///< 液位高度AD值
    float LiquidHeight;                 ///< 液位高度
    uint16_t PCapDA_ResultValue;        ///< 电容DA原始输出值
    uint16_t PCapDA_OutValue;           ///< 电容DA转换输出值
    uint32_t PCap_ResultValue;          ///< PCap原始值
}PCap_DataConvert_Out_Param;

/* 使用RT-Thread操作系统,USING_RT_THREAD_OS在main.h中定义 */
#ifdef USING_RT_THREAD_OS
#include <rtthread.h>

#define PCAP_DEVICE_NAME        "pcap"

struct rt_pcap_device_obj {
    struct rt_device            dev;
    DataFilterParam             DataFilter;
    PCap_DataConvert_Param      PCap_DataConvert;
    PCap_DataConvert_Out_Param  PCap_DataConvert_Out;
};
#endif


/**@brief       切换滤波水平函数
* @param[in]    FiltFactor : 滤波系数;
* @param[in]    FilterParam : 指定滤波参数结构体;
* @return       函数执行结果
* - None
* @note         滤波水平分9级，数字从低到高对应滤波深度从低到高
*/
void SwitchCurFilter(uint8_t FiltFactor, DataFilterParam *FilterParam);

#ifdef __IN_FLASH_APP_H

/**@brief       初始化数据滤波需要的参数结构
* @param[in]    Filterfactor_CountMax : 滤波系数和累计输入的最大值,滤波系数在高8位,
* 累计输入的最大值在低8位;
* @param[out]   FilterParam : 数据滤波需要的参数结构指针;
* @return       函数执行结果
* - None
* @note         要使用本函数,要加入In_Flash_app.c、In_Flash_app.h、
* In_Flash.ch和In_Flash.h文件
*/
void DataFilterParam_Init(DataFilterParam *FilterParam, uint16_t Filterfactor_CountMax);
#else

/**@brief       初始化数据滤波需要的参数结构
* @param[in]    Filterfactor_CountMax : 滤波系数和累计输入的最大值,滤波系数在高8位,
* 累计输入的最大值在低8位;
* @param[in]    InputRangeMax : 输入范围最大值;
* @param[in]    InputRangeMin : 输入范围最小值;
* @param[out]   FilterParam : 数据滤波需要的参数结构指针;
* @return       函数执行结果
* - None
*/
void DataFilterParam_Init(DataFilterParam *FilterParam, uint16_t Filterfactor_CountMax,
                            uint32_t InputRangeMax, uint32_t InputRangeMin);
#endif

#ifdef __IN_FLASH_APP_H

/**@brief       初始化数据转换需要的参数结构
* @param[out]   DataConvert_Param : 数据转换需要的参数结构指针;
* @return       函数执行结果
* - None
* @note         要使用本函数,要加入In_Flash_app.c、In_Flash_app.h、
* In_Flash.ch和In_Flash.h文件
*/
void DataConvertParam_Init(PCap_DataConvert_Param *DataConvert_Param);
#else

/**@brief       初始化数据转换需要的参数结构
* @param[in]    CompenEn : 补偿使能;
* @param[in]    Param : 要写入DataConvert_Param的参数数组指针;
* @param[in]    Param_Size : Param的大小;
* @param[out]   DataConvert_Param : 数据转换需要的参数结构指针;
* @return       函数执行结果
* - OP_SUCCESS(成功)
* - OP_FAILED(失败)
* @note         Param的数据分布(按字节分布): 0~1,位移高度; 2~5,测量电容零点;
* 6~9,测量电容满点; 10~13,测量电容零点; 14~17,测量电容满点; 18~19,电容AD值零点; 
* 20~21,电容AD值下刻度; 22~23,电容AD值上刻度; 24~25,电容AD值满量程; 
* 26~27,电容DA值零点; 28~29,电容DA值下刻度; 30~31,电容DA值上刻度; 
* 32~33,电容DA值满量程; 34~35,电容修正系数K; 36~37,电容修正系数B;
*/
uint8_t DataConvertParam_Init(PCap_DataConvert_Param *DataConvert_Param, uint8_t CompenEn,
                            uint8_t *Param, uint16_t Param_Size);
#endif

/**@brief       获取PCap原始采集值
* @param[in]    reg_addr : 结果寄存器的地址;
* @param[out]   PCap_Result : 保存PCap的输出结果;
* @return       函数执行结果
* - OP_SUCCESS(操作成功)
* - OP_FAILED(操作失败)
*/
uint8_t Sensor_PCap_GetResult(uint8_t reg_addr, uint32_t *PCap_Result);

/**@brief       数据按指定滤波参数进行滤波
* @param[in]    FilterParam : 指定滤波参数结构体;
* @param[in]    InputValue : 输入的数据;
* @param[out]   OutputValue : 输出数据的指针;
* @return       函数执行结果
* - OP_SUCCESS(操作成功)
* - OP_FAILED(操作失败)
* @note         使用本函数前,先将FilterParam初始化(即对它的每一个成员赋合适的值),否则不能正常使用
*/
uint8_t Sensor_DataFilter(DataFilterParam *FilterParam, uint32_t InputValue, uint32_t *OutputValue);

/**@brief       数据按指定控制参数进行数据转换
* @param[in]    DataConvert_Param : 指定控制参数结构体;
* @param[in]    InputValue : 输入的数据;
* @param[out]   DataConvert_Out : 输出数据的参数结构体指针
* @return       函数执行结果
* - None
* @note         使用本函数前,先将DataConvert_Param初始化(即对它的每一个成员赋合适的值),否则不能正常使用
*/
void Sensor_PCap_DataConvert(PCap_DataConvert_Param *DataConvert_Param, 
                                uint32_t InputValue, 
                                PCap_DataConvert_Out_Param *DataConvert_Out);

#ifdef __cplusplus
}
#endif
#endif
