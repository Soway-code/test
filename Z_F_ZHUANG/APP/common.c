#include "common.h"

void Delay_Ms(uint32_t cnt)
{
    cnt = cnt * 3200;
   

    while (cnt--);
}

void Delay_Us(uint32_t cnt)
{
    cnt = cnt * 8;
    //cnt = cnt * 2;
    while (cnt--);
}

void delay_us(uint32_t us)
{
    uint32_t delay = (HAL_RCC_GetHCLKFreq() / 4000000 * us);
    while(delay--)
    {
        ;
    }

}


//**************************************************************************************************
// 名称         : Unshort2Array()
// 创建日期     : 2018-06-08
// 作者         : MMX
// 功能         : unsigned short类型数据转换为数组
// 输入参数     : uint16_t SourceData       要转换的数据
// 输出参数     : uint8_t* Array            转换的数组
// 返回结果     : 无
// 注意和说明   : 无
// 修改内容     : 无 
//**************************************************************************************************
void Unshort2Array(uint16_t SourceData, uint8_t* Array)
{
    *Array = *((uint8_t*)&SourceData + 1);
    *(Array + 1) = *(uint8_t*)&SourceData;
}



//**************************************************************************************************
// 名称         : long32Array()
// 创建日期     : 2018-06-08
// 作者         : MMX
// 功能         : long类型数据转换为数组
// 输入参数     : uint32_t SourceData       要转换的数据
// 输出参数     : uint8_t* Array            转换的数组
// 返回结果     : 无
// 注意和说明   : 无
// 修改内容     : 无 
//**************************************************************************************************
void long32Array(uint32_t SourceData, uint8_t* Array)
{
  //  *Array = *((uint8_t*)&SourceData + 1);
   // *(Array + 1) = *(uint8_t*)&SourceData;
    
   *Array = *((uint8_t*)&SourceData + 3);
   *(Array + 1) = *((uint8_t*)&SourceData + 2);
   *(Array + 2) = *((uint8_t*)&SourceData + 1);
   *(Array + 3) = *(uint8_t*)&SourceData;

}


//**************************************************************************************************
// 名称         : Unlong2Array()
// 创建日期     : 2018-06-08
// 作者         : MMX
// 功能         : uint32_t类型数据转换为数组
// 输入参数     : uint32_t SourceData       要转换的数据
// 输出参数     : uint8_t* Array            转换的数组
// 返回结果     : 无
// 注意和说明   : 无
// 修改内容     : 无 
//**************************************************************************************************
void Unlong2Array(uint32_t SourceData, uint8_t* Array)
{
    uint8_t i;
    for(i = 0; i < 4; i++)
    {
        *(Array + i) = *((uint8_t*)&SourceData + 3 - i);
    }
}




