/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   change to new framework
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#include "mb.h"
#include "user_mb_app.h"
#include "task.h"


/* defined the LED pin: PB4 */
#define LED0_PIN     GET_PIN(B, 4)

int main(void)
{
    /* set LED0 pin mode to output */
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
	u8g2_ssd1306_12864_sw_i2c_example();
	mb_slave_samlpe();
	task_init();	
	return RT_EOK;
}
