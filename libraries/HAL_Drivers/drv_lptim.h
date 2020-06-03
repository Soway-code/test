/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author          Notes
 * 2019-05-06     Zero-Free       first version
 */

#ifndef  __DRV_PMTIMER_H__
#define  __DRV_PMTIMER_H__

#include <rtthread.h>
#include <board.h>

#ifdef RT_USING_PM

#ifdef LPTIM1
  #define PM_USING_LPTIM
#elif defined(SOC_SERIES_STM32F1)
  #define PM_USING_RTC_ALARM_F1
#elif !defined(RTC_CR_WUTIE) && defined(RTC_SSR_SS)
  #define PM_USING_RTC_ALARM_SSR
#elif defined(RTC_SSR_SS)
  #define PM_USING_RTC_WAKEUPTIM_SSR
#else
  #define PM_USING_RTC_WAKEUPTIM_NO_SSR
#endif

rt_uint32_t stm32_lptim_get_countfreq(void);
rt_uint32_t stm32_lptim_get_tick_max(void);
rt_uint32_t stm32_lptim_get_current_tick(void);

rt_err_t stm32_lptim_start(rt_uint32_t load);
void stm32_lptim_stop(void);

#endif
#endif /* __DRV_PMTIMER_H__ */
