#ifndef __TASK_H
#define __TASK_H

#include "board.h"
#include "rtthread.h"
#include "main.h"
#include "oled.h"
#include "ds18b20.h"
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "In_Flash_app.h"
#include "ModBusRtu.h"
#include "stdio.h"
#include "tim.h"
uint8_t task_init(void);

#endif 
