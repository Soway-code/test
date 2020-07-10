#include "iwdg.h"


IWDG_HandleTypeDef  IWDG_HandleStructure;

//******************************************************************************
// 名称         : User_Iwdg_Init()
// 创建日期     : 2018-06-08
// 作者         : MMX
// 功能         : 看门狗配置
// 输入参数     : 无
// 输出参数     : 无
// 返回结果     : 无
// 注意和说明   : 无
// 修改内容     : 无 
//******************************************************************************
void User_Iwdg_Init(void)
{
    IWDG_HandleStructure.Init.Prescaler = IWDG_PRESCALER_8;                     //LSI的32.768的8分频位4.096K
    IWDG_HandleStructure.Init.Reload = 0x0FA0;                                  //重装载值为4000，约1s
    IWDG_HandleStructure.Init.Window = 0x0FA0;
    IWDG_HandleStructure.Instance = IWDG;
    HAL_IWDG_Init(&IWDG_HandleStructure);
}


//******************************************************************************
// 名称         : User_Iwdg_Feed()
// 创建日期     : 2018-06-08
// 作者         : MMX
// 功能         : 喂狗
// 输入参数     : 无
// 输出参数     : 无
// 返回结果     : 无
// 注意和说明   : 无
// 修改内容     : 无 
//******************************************************************************
void User_Iwdg_Feed(void)
{
    HAL_IWDG_Refresh(&IWDG_HandleStructure);
}

