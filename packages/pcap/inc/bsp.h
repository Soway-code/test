#ifndef _BSP_H_
#define _BSP_H_

#include "stm32l0xx.h"
#include "eeprom.h"
//#include "Bsp_init.h"
//#include "nvic.h"
//#include "delay.h"
#include "drv_usart.h"
//#include "led.h"
#include "Capacitive.h"
//#include "SoftwareIIC.h"
#include "cap_parameter.h"
#include "DA.h"
#include "Filter.h"
#include "modbus.h"
#include "PCap.h"
#include "type.h"
#include "algorithm.h"
//#include "sys_cfg.h"
//#include "Led.h"
//#include "MMA8652.h"
//#include "MEMS.h"
#include "Modbus.h"
#include "modbus_ascii.h"
#include <stdlib.h>
#include <stdio.h>


//#include  <includes.h>

//extern OS_TCB          AppTaskMBTCB;
//extern OS_TCB          AppFilterTCB;
//extern OS_TCB          AppMEMSTCB;


extern void RunVar_Init(void);

//void Dev_Init();

#endif
