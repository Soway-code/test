/**
* @file         main.c
* @author       weijianx
* @date         2020-07-02
* @version      V1.0.0
* @copyright    2020-2030,深圳市信为科技发展有限公司
**********************************************************************************
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/07/02  <td>1.0.0    <td>weijianx  <td>创建初始版本
* </table>
*
**********************************************************************************
*/
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "task.h"

////Remap address define
//#define APPLICATION_ADDRESS     (uint32_t)0x8004000

////Define variable
//    #if   (defined ( __CC_ARM ))
//    __IO uint32_t VectorTable[48] __attribute__((at(0x20000000)));
//    #elif (defined (__ICCARM__))
//    #pragma location = 0x20000000
//    __no_init __IO uint32_t VectorTable[48];
//    #elif defined   (  __GNUC__  )
//    __IO uint32_t VectorTable[48] __attribute__((section(".RAMVectorTable")));
//    #elif defined ( __TASKING__ )
//    __IO uint32_t VectorTable[48] __at(0x20000000);
//    #endif

//void RemapInterruptVerctor( void )
//{
//    for(uint32_t i = 0; i < 48; i++)
//    {
//        VectorTable[i] = *(__IO uint32_t*)(APPLICATION_ADDRESS + (i<<2));
//    }
//	
//	//开启 SYSCFG 时钟
//	__HAL_RCC_SYSCFG_CLK_ENABLE();
//	//重映射 SRAM 地址到 0x00000000
//	__HAL_SYSCFG_REMAPMEMORY_SRAM();
//}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
//	適用于没有中断向量表重映射寄存器的芯片
//    RemapInterruptVerctor();
	task_Init();

	while (1)
	{
		rt_thread_mdelay(1000);

	}
	/* USER CODE END 3 */
}




/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
