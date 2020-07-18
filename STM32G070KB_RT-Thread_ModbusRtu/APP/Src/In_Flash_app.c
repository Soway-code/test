/**@file        In_Flash_app.c
* @brief        读写内部Flash的应用
* @details      从STM32F072的内部Flash中读取或写入系统内部参数
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

#include "In_Flash_app.h"


/** 默认系统参数 */
const uint8_t User_Default_Param[PRO_DEFAULT_LEN] = 
{
    0x01,                                               //参数初始化标志位

    0x01,                                               //设备地址
    0x01,                                               //波特率(9600)
    0x00,                                               //奇偶校验(无校验)
    0x01,                                               //滤波系数(10s)
    0x00,                                               //自动发送周期
    0x00,                                               //补偿使能
    0x00,                                               //是否冻结设备
    0x00,                                               //输出方式

    0x00,0x94,                                          //量程默认148(mm)

    0x00,0x19,0x43,0x4E,                                //零点电容值      1655630
    0x00,0x25,0x07,0xEF,                                //满量程电容值    2426863
    0x00,0x19,0x43,0x4E,                                //零点电容备份    1655630
    0x00,0x25,0x07,0xEF,                                //满量程电容备份  2426863

    0x00,0x01,                                          //电容零点AD值    0
    0x51,0xA4,                                          //电容下刻度AD值  20900
    0xB7,0x98,                                          //电容上刻度AD值  47000
    0xFF,0xFF,                                          //电容满量程AD值  65535

    0x00,0x82,                                          //电容零点DA值   130
    0x02,0x44,                                          //电容下刻度DA值 580
    0x03,0xA2,                                          //电容上刻度DA值 930
    0x04,0x6A,                                          //电容满度DA值   1130 
    0x00,0x00,                                          //温度零点DA值
    0x0F,0xFF,                                          //温度满度DA值

    0x00,0x64,                                          //电容修正系数K
    0x00,0x64,                                          //电容修正系数B
    0x00,0x64,                                          //温度1修正系数K1
    0x00,0x64,                                          //温度1修正系数B1
    0x00,0x64,                                          //温度2修正系数K2
    0x00,0x64,                                          //温度2修正系数B2
    
    SYSTEMPARAM_IS_PROGRAMED                            //写入初始值标志                                                                    
};

/**@brief       向STM32F072xx内部Flash指定位置写多个字节且备份3份
* @param[in]    FlashAddr : 写起始地址
* @param[in]    pWrbuf : 要写入的数据缓存指针;
* @param[in]    Wrlen : 数据长度
* @return       函数执行结果
* - OP_SUCCESS(成功)
* - OP_FAILED(失败)
*/
uint8_t InFlash_Write3T_MultiBytes(uint16_t FlashAddr, const uint8_t *pWrbuf, uint16_t Wrlen)
{
    //错误状态
    uint8_t Err;
    //写状态
    uint8_t Wrsta;
    //写起始地址
    uint16_t EepAddress;

    Err = OP_SUCCESS;
    Wrsta = OP_SUCCESS;
    EepAddress = FlashAddr;

    //系统参数存储区
    Wrsta = InFlash_Write_MultiBytes(EepAddress, pWrbuf, Wrlen);
    if(OP_SUCCESS != Wrsta)
    {
        Err = OP_FAILED;
    }
    //系统参数备份区1
    Wrsta = InFlash_Write_MultiBytes(SYSTEM_PARAM_BAK1 + EepAddress, pWrbuf, Wrlen);
    if(OP_SUCCESS != Wrsta)
    {
        Err = OP_FAILED;
    }
    //系统参数备份区2
    Wrsta = InFlash_Write_MultiBytes(SYSTEM_PARAM_BAK2 + EepAddress, pWrbuf, Wrlen);
    if(OP_SUCCESS != Wrsta)
    {
        Err = OP_FAILED;
    }

    return Err;
}

/**@brief       内部Flash系统参数检查,若出现不一致的参数,重新将User_Default_Param
* 写入内部Flash
* @param[in]    Cur_Param : 要检查的参数数组;
* @param[in]    Num_Of_Cur_Param : 参数数组的长度;
* @return       函数执行结果
* - None
*/
void InFlash_SystemParam_Check(uint8_t *Cur_Param, uint16_t Num_Of_Cur_Param)
{
    uint16_t Cnt;
    uint8_t Check_Sta;
    uint8_t Cur_Param_Bak1;
    uint8_t Cur_Param_Bak2;

    for(Cnt = 0; Cnt < Num_Of_Cur_Param; Cnt++)
    {
        //检查状态
        Check_Sta = 0;
        //备份1
        Cur_Param_Bak1 = InFlash_Read_OneByte((SYSTEM_PARAM_BAK1 + Cnt));
        //备份2
        Cur_Param_Bak2 = InFlash_Read_OneByte((SYSTEM_PARAM_BAK2 + Cnt));
        //当前参数与备份1不同
        if(Cur_Param[Cnt] != Cur_Param_Bak1)
        {
            Check_Sta |= 0x01;
        }
        //备份1与备份2不同
        if(Cur_Param_Bak1 != Cur_Param_Bak2)
        {
            Check_Sta |= 0x02;
        }
        //当前参数与备份2不同
        if(Cur_Param[Cnt] != Cur_Param_Bak2)
        {
            Check_Sta |= 0x04;
        }
        //备份2有误
        if(0x06 == Check_Sta)
        {
            InFlash_Write_OneByte((SYSTEM_PARAM_BAK2 + Cnt), Cur_Param[Cnt]);
        }
        //备份1有误
        else if(0x03 == Check_Sta)
        {
            InFlash_Write_OneByte((SYSTEM_PARAM_BAK1 + Cnt), Cur_Param[Cnt]);
        }
        //当前参数有误
        else if(0x05 == Check_Sta)
        {
            InFlash_Write_OneByte((RUN_ADDR_BASE + Cnt), Cur_Param_Bak1);
        }
        else 
        {
            if(0x00 != Check_Sta)
            {
            break;
            }
        }
    }

    if(InFlash_Read_OneByte(SYSTEMPARAM_PROGRAMED) != SYSTEMPARAM_IS_PROGRAMED)
    {
        goto write_systemparam;
    }
    if(InFlash_Read_OneByte(SYSTEM_PARAM_BAK1 + SYSTEMPARAM_PROGRAMED) != SYSTEMPARAM_IS_PROGRAMED)
    {
        goto write_systemparam;
    }
    if(InFlash_Read_OneByte(SYSTEM_PARAM_BAK2 + SYSTEMPARAM_PROGRAMED) != SYSTEMPARAM_IS_PROGRAMED)
    {
        goto write_systemparam;
    }
    //如果三份数据全不对就写入缺省系统参数
    if((0x01 == Check_Sta) || (0x02 == Check_Sta) || (0x04 == Check_Sta) || (0x07 == Check_Sta))
    {
write_systemparam:
        //写入系统参数缺省值到三份地址区
        InFlash_Write3T_MultiBytes(RUN_ADDR_BASE, User_Default_Param, Num_Of_Cur_Param);
    }
}

/**@brief       读取并检查保存在内部Flash的设备参数
* @return       函数执行结果
* - None
*/
void Check_Device_Param(void)
{
    uint8_t InFlashInitFlg;
    uint16_t ParaTemp;
    uint8_t Device_Param[PRO_DEFAULT_LEN];

    //读取系统参数初始化标志
    InFlashInitFlg = InFlash_Read_OneByte(RUN_ADDR_BASE);

    //如果没有初始化就写入用户基本参数，标定的参数不能动
    if(InFlashInitFlg != User_Default_Param[RUN_ADDR_BASE])
    {
        ParaTemp = 0;
        while(OP_SUCCESS != InFlash_Write3T_MultiBytes(RUN_ADDR_BASE, User_Default_Param, USER_DEFAULT_LEN))
        {
            ParaTemp++;
            if(WRITE_FLASH_ERR_MAX < ParaTemp)
            {
            break;
            }
        }
    }

    //读取系统参数，存入Device_Param缓存
    InFlash_Read_MultiBytes(RUN_ADDR_BASE, Device_Param, PRO_DEFAULT_LEN);

    //检查系统参数
    InFlash_SystemParam_Check(Device_Param, PRO_DEFAULT_LEN);
}

