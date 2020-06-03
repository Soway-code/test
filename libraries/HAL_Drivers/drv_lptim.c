/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author          Notes
 * 2019-05-06     Zero-Free       first version
 * 2020-03-20     Sunwancn        ports stm32 all
 */

#include <board.h>

#if defined(RT_USING_PM)

#include <drv_lptim.h>
#ifdef SOC_SERIES_STM32F1
#include <stm32f1xx_ll_rtc.h>
#endif /* SOC_SERIES_STM32F1 */

#if defined(PM_USING_LPTIM)

static LPTIM_HandleTypeDef LptimHandle;

void LPTIM1_IRQHandler(void)
{
    HAL_LPTIM_IRQHandler(&LptimHandle);
}

#else

/* -- PM timer use RTC -- */
RTC_HandleTypeDef RtcHandle = {0};

#if defined(PM_USING_RTC_ALARM_SSR)

RTC_AlarmTypeDef RtcAlarm = {0};
static RTC_TimeTypeDef Last_time = {0};

#elif defined(PM_USING_RTC_ALARM_F1)

static rt_uint32_t Last_timecounter = 0;

#else

static RTC_TimeTypeDef Last_time = {0};

#endif /* defined(PM_USING_RTC_ALARM_SSR) */

/**
  * @brief This function handles RTC Interrupt through EXTI.
  */
#if defined(SOC_SERIES_STM32F0)

void RTC_IRQHandler(void)
{
    HAL_RTC_AlarmIRQHandler(&RtcHandle);

#ifdef PM_USING_RTC_WAKEUPTIM_SSR
    HAL_RTCEx_WakeUpTimerIRQHandler(&RtcHandle);
#endif
}

#elif defined(SOC_SERIES_STM32F1)

void RTC_Alarm_IRQHandler(void)
{
    HAL_RTC_AlarmIRQHandler(&RtcHandle);
}

#else

void RTC_WKUP_IRQHandler(void)
{
    HAL_RTCEx_WakeUpTimerIRQHandler(&RtcHandle);
}

#endif /* defined(SOC_SERIES_STM32F0) */
/* end -- PM timer use RTC -- */

#endif /* defined(PM_USING_LPTIM) */

/**
 * This function get the count clock of LPTIM or RTC
 *
 * @return the count clock frequency in Hz
 */
RT_WEAK rt_uint32_t HAL_PM_GetCounterFreq(void)
{
#if defined(PM_USING_LPTIM)

    return HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_LPTIM1) \
           / (((LptimHandle.Instance->CFGR & LPTIM_CFGR_PRESC) >> LPTIM_CFGR_PRESC_Pos) + 1U);

#elif defined(PM_USING_RTC_ALARM_SSR)

    return HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_RTC) \
           / (((RtcHandle.Instance->PRER & RTC_PRER_PREDIV_A) >> RTC_PRER_PREDIV_A_Pos) + 1U);

#elif defined(PM_USING_RTC_ALARM_F1)

    return HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_RTC) / (RtcHandle.Instance->PRLL + 1U);

#else

    return HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_RTC) / 16U;

#endif /* defined(PM_USING_LPTIM) */
}

/**
 * This function initialize the LPTIM or RTC
 */
RT_WEAK int HAL_PM_CounterInit(void)
{
#ifdef PM_USING_LPTIM
    LptimHandle.Instance = LPTIM1;
    LptimHandle.Init.Clock.Source = LPTIM_CLOCKSOURCE_APBCLOCK_LPOSC;
    LptimHandle.Init.Clock.Prescaler = LPTIM_PRESCALER_DIV32;
    LptimHandle.Init.Trigger.Source = LPTIM_TRIGSOURCE_SOFTWARE;
    LptimHandle.Init.OutputPolarity = LPTIM_OUTPUTPOLARITY_HIGH;
    LptimHandle.Init.UpdateMode = LPTIM_UPDATE_IMMEDIATE;
    LptimHandle.Init.CounterSource = LPTIM_COUNTERSOURCE_INTERNAL;
    if (HAL_LPTIM_Init(&LptimHandle) != HAL_OK)
    {
        return -1;
    }

    NVIC_ClearPendingIRQ(LPTIM1_IRQn);
    NVIC_SetPriority(LPTIM1_IRQn, 0);
    NVIC_EnableIRQ(LPTIM1_IRQn);

    return 0;
#else
    /* Initialize RTC */
    rt_uint32_t rtcClkFreq = HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_RTC);

    RtcHandle.Instance = RTC;

#if defined(PM_USING_RTC_ALARM_F1)
    RtcHandle.Init.AsynchPrediv = rtcClkFreq / 1024U - 1U;
    RtcHandle.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
#elif defined(PM_USING_RTC_WAKEUPTIM_NO_SSR)
    RtcHandle.Init.AsynchPrediv = rtcClkFreq / 1024U - 1U;
    RtcHandle.Init.SynchPrediv = 0;
    RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
    RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
    RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
#else
    RtcHandle.Init.SynchPrediv = 1023U;
    RtcHandle.Init.AsynchPrediv = rtcClkFreq / 1024U - 1U;
    RtcHandle.Init.SynchPrediv = rtcClkFreq / (RtcHandle.Init.AsynchPrediv + 1U) - 1U;
    RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
    RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
    RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
#endif /* defined(PM_USING_RTC_ALARM_F1) */

    if (HAL_RTC_Init(&RtcHandle) != HAL_OK)
    {
        return -1;
    }

#if defined(PM_USING_RTC_ALARM_SSR)
    /* Enable the Alarm A */
    RtcAlarm.AlarmTime.Hours = 0x0;
    RtcAlarm.AlarmTime.Minutes = 0x0;
    RtcAlarm.AlarmTime.Seconds = 0x0;
    RtcAlarm.AlarmTime.SubSeconds = 0x0;
    RtcAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    RtcAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
    RtcAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY | RTC_ALARMMASK_HOURS;
    RtcAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
    RtcAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    RtcAlarm.AlarmDateWeekDay = 0x1;
    RtcAlarm.Alarm = RTC_ALARM_A;
    if (HAL_RTC_SetAlarm(&RtcHandle, &RtcAlarm, RTC_FORMAT_BCD) != HAL_OK)
    {
        return -1;
    }
#endif /* defined(PM_USING_RTC_ALARM_SSR) */

#if defined(SOC_SERIES_STM32F0)
    NVIC_ClearPendingIRQ(RTC_IRQn);
    NVIC_SetPriority(RTC_IRQn, 0);
    NVIC_EnableIRQ(RTC_IRQn);
#elif defined(SOC_SERIES_STM32F1)
    NVIC_ClearPendingIRQ(RTC_Alarm_IRQn);
    NVIC_SetPriority(RTC_Alarm_IRQn, 0);
    NVIC_EnableIRQ(RTC_Alarm_IRQn);
#else
    NVIC_ClearPendingIRQ(RTC_WKUP_IRQn);
    NVIC_SetPriority(RTC_WKUP_IRQn, 0);
    NVIC_EnableIRQ(RTC_WKUP_IRQn);
#endif /* defined(SOC_SERIES_STM32F0) */

    return 0;
#endif /* ! PM_USING_LPTIM */
}

/**
 * This function get current count value of LPTIM or RTC
 *
 * @return the count vlaue
 */
rt_uint32_t stm32_lptim_get_current_tick(void)
{
#if defined(PM_USING_LPTIM)

    return HAL_LPTIM_ReadCounter(&LptimHandle);

#elif defined(PM_USING_RTC_ALARM_F1)

    return LL_RTC_TIME_Get(RtcHandle.Instance) - Last_timecounter;

#elif defined(PM_USING_RTC_WAKEUPTIM_NO_SSR)

    /* No subsecond
     * Actual seconds are (AsynchPrediv + 1)/((AsynchPrediv_1S + 1)*(SynchPrediv_1S + 1)) of the original
     */
    RTC_TimeTypeDef *ctime;
    rt_uint32_t datetmpreg;
    rt_uint32_t seconds;
    rt_uint32_t last_seconds = Last_time.Hours * 3600U + Last_time.Minutes * 60U + Last_time.Seconds;

    HAL_RTC_GetTime(&RtcHandle, ctime, RTC_FORMAT_BIN);
    datetmpreg = RtcHandle.Instance->DR;
    (void)datetmpreg;

    seconds = ctime->Hours * 3600U + ctime->Minutes * 60U + ctime->Seconds;
    if (seconds < last_seconds)
        seconds += 24 * 3600U - last_seconds;
    else
        seconds -= last_seconds;

    /* Convert to wakeup timer tick */
    return seconds * (((RtcHandle.Instance->PRER & RTC_PRER_PREDIV_A) >> RTC_PRER_PREDIV_A_Pos) + 1U) \
           * ((RtcHandle.Instance->PRER & RTC_PRER_PREDIV_S) + 1U) / 16U;

#else

    /* Can't read the wakeuptime counter, then read the subsecond to calculating the elapsed time */
    RTC_TimeTypeDef *ctime;
    rt_uint32_t datetmpreg;
    rt_uint32_t seconds;
    rt_int32_t subseconds;

    HAL_RTC_GetTime(&RtcHandle, ctime, RTC_FORMAT_BIN);
    datetmpreg = RtcHandle.Instance->DR;
    (void)datetmpreg;

    if (ctime->Seconds < Last_time.Seconds)
        seconds = 60U + ctime->Seconds - Last_time.Seconds;
    else
        seconds = ctime->Seconds - Last_time.Seconds;

    /* Subsecond is count down */
    subseconds = (rt_int32_t)Last_time.SubSeconds - (rt_int32_t)ctime->SubSeconds;
    subseconds += seconds * (Last_time.SecondFraction + 1U);
#ifdef PM_USING_RTC_WAKEUPTIM_SSR
    /* Convert to wakeup timer tick */
    subseconds = subseconds * (((RtcHandle.Instance->PRER & RTC_PRER_PREDIV_A) >> RTC_PRER_PREDIV_A_Pos) + 1U) / 16U;
#endif /* PM_USING_RTC_WAKEUPTIM_SSR */

    return (rt_uint32_t)subseconds;

#endif
}

/**
 * This function get the max value that LPTIM can count
 *
 * @return the max count
 */
rt_uint32_t stm32_lptim_get_tick_max(void)
{
    return (0xFFFF);
}

/**
 * This function start LPTIM or RTC with reload value
 *
 * @param reload The value that LPTIM count down from
 *
 * @return RT_EOK
 */
rt_err_t stm32_lptim_start(rt_uint32_t reload)
{
#if defined(PM_USING_LPTIM)

    HAL_LPTIM_TimeOut_Start_IT(&LptimHandle, 0xFFFF, reload);

#elif defined(PM_USING_RTC_ALARM_F1)

    /* Set the alarm counter and enable wakeup from stop mode */
    Last_timecounter = LL_RTC_TIME_Get(RtcHandle.Instance);
    __HAL_LOCK(&RtcHandle);
    RtcHandle.State = HAL_RTC_STATE_BUSY;
    LL_RTC_ALARM_SetCounter(RtcHandle.Instance, Last_timecounter + reload);
    __HAL_RTC_ALARM_CLEAR_FLAG(&RtcHandle, RTC_FLAG_ALRAF);
    __HAL_RTC_ALARM_ENABLE_IT(&RtcHandle, RTC_IT_ALRA);
    __HAL_RTC_ALARM_EXTI_ENABLE_IT();
    __HAL_RTC_ALARM_EXTI_ENABLE_RISING_EDGE();
    RtcHandle.State = HAL_RTC_STATE_READY;
    __HAL_UNLOCK(&RtcHandle);

#elif defined(PM_USING_RTC_ALARM_SSR)

    rt_uint32_t datetmpreg;
    rt_int32_t timeout = 0;
    rt_uint32_t seconds = 0;
    rt_uint32_t minutes = 0;

    /* Get the current time */
    HAL_RTC_GetTime(&RtcHandle, &Last_time, RTC_FORMAT_BIN);
    datetmpreg = RtcHandle.Instance->DR;
    (void)datetmpreg;

    seconds = Last_time.Seconds;
    minutes = Last_time.Minutes;

    seconds += reload / (Last_time.SecondFraction + 1U);
    reload = reload % (Last_time.SecondFraction + 1U);

    /* Subsecond is count down */
    timeout = (rt_int32_t)Last_time.SubSeconds - (rt_int32_t)reload;
    if (timeout < 0)
    {
        timeout += Last_time.SecondFraction + 1;
        seconds ++;
    }

    while (seconds > 59U)
    {
        seconds -= 60U;

        if (minutes == 59U)
            minutes = 0U;
        else
            minutes++;
    }

    RtcAlarm.AlarmTime.Seconds = seconds;
    RtcAlarm.AlarmTime.Minutes = minutes;
    RtcAlarm.AlarmTime.SubSeconds = (rt_uint32_t)timeout;

    HAL_RTC_DeactivateAlarm(&RtcHandle, RTC_ALARM_A);
    HAL_RTC_SetAlarm_IT(&RtcHandle, &RtcAlarm, RTC_FORMAT_BIN);

#else

    rt_uint32_t datetmpreg;
    /* PM_USING_RTC_WAKEUPTIM_SSR */
    HAL_RTCEx_SetWakeUpTimer_IT(&RtcHandle, reload, RTC_WAKEUPCLOCK_RTCCLK_DIV16);

    HAL_RTC_GetTime(&RtcHandle, &Last_time, RTC_FORMAT_BIN);
    datetmpreg = RtcHandle.Instance->DR;
    (void)datetmpreg;

#endif /* defined(PM_USING_LPTIM) */

    return (RT_EOK);
}

/**
 * This function stop LPTIM or RTC
 */
void stm32_lptim_stop(void)
{
#if defined(PM_USING_LPTIM)

    rt_uint32_t _ier;

    _ier = LptimHandle.Instance->IER;
    LptimHandle.Instance->ICR = LptimHandle.Instance->ISR & _ier;

#elif defined(PM_USING_RTC_ALARM_SSR) || defined(PM_USING_RTC_ALARM_F1)

    __HAL_RTC_ALARM_DISABLE_IT(&RtcHandle, RTC_IT_ALRA);

#else

    /* PM_USING_RTC_WAKEUPTIM_SSR */
    __HAL_RTC_WAKEUPTIMER_DISABLE_IT(&RtcHandle, RTC_IT_WUT);

#endif /* defined(PM_USING_LPTIM) */
}

rt_uint32_t stm32_lptim_get_countfreq(void)
{
    return HAL_PM_GetCounterFreq();
}

/**
 * This function initialize the lptim
 */
int stm32_hw_lptim_init(void)
{
#ifdef PM_USING_LPTIM
    LptimHandle.Instance = LPTIM1;
#else
    RtcHandle.Instance = RTC;
#endif
    return HAL_PM_CounterInit();
}

INIT_DEVICE_EXPORT(stm32_hw_lptim_init);

#ifdef BSP_USING_ONCHIP_RTC
/**
 * This function reinitialize the RTC on app initialization
 */
int app_rtc_init(void)
{
    return HAL_PM_CounterInit();
}
INIT_APP_EXPORT(app_rtc_init);
#endif /* BSP_USING_ONCHIP_RTC */

#endif /* defined(RT_USING_PM) */
