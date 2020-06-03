/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                             (c) Copyright 2012; Micrium, Inc.; Weston, FL
*
*                   All rights reserved.  Protected by international copyright laws.
*                   Knowledge of the source code may not be used to write a similar
*                   product.  This file may only be used in accordance with a license
*                   and should not be redistributed in any way.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                      APPLICATION CONFIGURATION
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                           STM32F0518-EVAL
*                                          Evaluation Board
*
* Filename      : app_cfg.h
* Version       : V1.00
* Programmer(s) : MD
*********************************************************************************************************
*/

#ifndef  APP_CFG_MODULE_PRESENT
#define  APP_CFG_MODULE_PRESENT

/*
*********************************************************************************************************
*                                       MODULE ENABLE / DISABLE
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/
#define  APP_CFG_TASK_START_PRIO                          	2u
#define  APP_CFG_TASK_Filter_PRIO				3u
#define  APP_CFG_TASK_MB_PRIO                             	4u
#define  APP_CFG_TASK_MEMS_PRIO                           	5u

/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/
#define  APP_CFG_TASK_START_STK_SIZE                    		64u
#define  APP_CFG_TASK_MB_STK_SIZE                       		96u
#define  APP_CFG_TASK_Filter_STK_SIZE				    	160u
#define  APP_CFG_TASK_MEMS_STK_SIZE					96u

/*
*********************************************************************************************************
*                                            TASK STACK SIZES LIMIT
*********************************************************************************************************
*/

#define  APP_CFG_TASK_START_STK_SIZE_PCT_FULL				90u
#define  APP_CFG_TASK_START_STK_SIZE_LIMIT					(APP_CFG_TASK_START_STK_SIZE     * (100u - APP_CFG_TASK_START_STK_SIZE_PCT_FULL))    / 100u

#define  APP_CFG_TASK_MB_STK_SIZE_PCT_FULL					90u
#define  APP_CFG_TASK_MB_STK_SIZE_LIMIT						(APP_CFG_TASK_MB_STK_SIZE      * (100u - APP_CFG_TASK_MB_STK_SIZE_PCT_FULL))   / 100u

#define  APP_CFG_TASK_Filter_STK_SIZE_PCT_FULL				90u
#define  APP_CFG_TASK_Filter_STK_SIZE_LIMIT					(APP_CFG_TASK_Filter_STK_SIZE      * (100u - APP_CFG_TASK_Filter_STK_SIZE_PCT_FULL))   / 100u

#define  APP_CFG_TASK_MEMS_STK_SIZE_PCT_FULL				90u
#define  APP_CFG_TASK_MEMS_STK_SIZE_LIMIT					(APP_CFG_TASK_MEMS_STK_SIZE      * (100u - APP_CFG_TASK_MEMS_STK_SIZE_PCT_FULL))   / 100u

/*
*********************************************************************************************************
*                                        uC/LIB CONFIGURATION
*********************************************************************************************************
*/

#define  LIB_MEM_CFG_OPTIMIZE_ASM_EN            DEF_DISABLED
#define  LIB_MEM_CFG_ARG_CHK_EXT_EN             DEF_ENABLED
#define  LIB_MEM_CFG_ALLOC_EN                   DEF_ENABLED
#define  LIB_MEM_CFG_HEAP_SIZE                  (1 * 1024L)

/*
*********************************************************************************************************
*                                     TRACE / DEBUG CONFIGURATION
*********************************************************************************************************
*/

#define  TRACE_LEVEL_OFF                               0
#define  TRACE_LEVEL_INFO                              1
#define  TRACE_LEVEL_DBG                               2


#define  APP_TRACE_LEVEL                        TRACE_LEVEL_OFF
#define  APP_TRACE                               printf

#define  APP_TRACE_INFO(x)               ((APP_TRACE_LEVEL >= TRACE_LEVEL_INFO)  ? (void)(APP_TRACE x) : (void)0)
#define  APP_TRACE_DBG(x)                ((APP_TRACE_LEVEL >= TRACE_LEVEL_DBG)   ? (void)(APP_TRACE x) : (void)0)

#endif
