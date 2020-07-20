/****************************************************************************************************************************************************************
** 版权:          2017-2027,深圳市信为科技发展有限公司
** 文件名:        VariaType.h
** 作者:          庄明群
** 版本:          V1.0.0
** 日期:          2017-05-16
** 描述:          变量类型
** 功能:          模块化变量
*****************************************************************************************************************************************************************
** 修改者:        No
** 版本:          No
** 修改内容:      No
** 日期:          No
****************************************************************************************************************************************************************/

#ifndef __VARIATYPE_H
#define __VARIATYPE_H
#ifdef __cplusplus
 extern "C" {
#endif


#define  OP_SUCCESS                 0x00                                       //操作成功
#define  OP_FAILED                  0x01                                       //操作失败

#define  UPDATA_NONE                0x00                                       //没有更新
#define  UPDATA_OK                  0x01                                       //更新完成

#define  CNT_MAX                    0x09                                       //最大计数
#define  INIT_FLAG                  0x80                                       //初始化设置

#define  CALIB_CLEAR                0x00                                       //标定清除

#define  COMPENSATE_ENABLE          0x01                                       //补偿使能
#define  COMPENSATE_DISABLE         0x00                                       //补偿禁止

#define  PCAP_FRESH_FLAG            0x0F                                       //PCAP更新数据

#define DAOUTCLIB_ENABLE                      0x0F                              //DAC输出标定使能
#define DAOUTCLIB_DISABLE                     0x00                              //DAC输出标定禁止


#ifdef __cplusplus
}
#endif
#endif
