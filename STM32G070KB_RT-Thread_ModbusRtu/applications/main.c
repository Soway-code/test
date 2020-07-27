/* USER CODE BEGIN Header */
/**@file      	main.c
* @brief     	Main program body
* @default		frame of project main function enter
* @author		loodao any question please send mail to 893204847@qq.com
* @date 		2020-07-17
* @version		V1.0
* @copyright	
************************************************************************
* @attention
* @brief hardware platefrom:stm32G070KB
* @brief OS Version: RT-Thread 4.0.2
* @brief para modify log:
* <table>
* <tr><th>Date	<th>Version 
* <tr><td>2020/07/08 <td>1.0 <td>Loodao <td> buidling new version
* </table>
*
***********************************************************************
*/

#include "task.h"

/* defined the LED pin: PB4 */
#define LED0_PIN     GET_PIN(B, 4)
#define USING_RT_THREAD_OS



int main(void)
{
    /// set LED0 pin mode to output 
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
	
	if(RT_ERROR == task_init())						///初始化创建线程
	{
		rt_kprintf("Thread creation failed!!!!!!\n");
		while(1) ;
	}
    return RT_EOK;
}
