/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-05-06     Zero-Free    first version
 * 2020-03-20     Sunwancn     ports stm32 all
 */

#include <board.h>

#ifdef RT_USING_PM

#include <drv_lptim.h>

static void uart_console_reconfig(void)
{
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
//#if RT_USING_CONSOLE
    rt_device_control(rt_console_get_device(), RT_DEVICE_CTRL_CONFIG, &config);
//#endif
}

/**
 * This function will put STM32 into sleep mode.
 *
 * @param pm pointer to power manage structure
 */
static void sleep(struct rt_pm *pm, rt_uint8_t mode)
{
    switch (mode)
    {
    case PM_SLEEP_MODE_NONE:
        break;

    case PM_SLEEP_MODE_IDLE:
    case PM_SLEEP_MODE_LIGHT:
        if (pm->run_mode == PM_RUN_MODE_LOW_SPEED)
        {
            /* Enter LP SLEEP Mode, Enable low-power regulator */
            HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON, PWR_SLEEPENTRY_WFI);
        }
        else
        {
            /* Enter SLEEP Mode, Main regulator is ON */
            HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
        }
        break;

    case PM_SLEEP_MODE_DEEP:
        /* Disable SysTick interrupt */
        CLEAR_BIT(SysTick->CTRL, (rt_uint32_t)SysTick_CTRL_TICKINT_Msk);
#if defined(SOC_SERIES_STM32L4)
        if (pm->run_mode == PM_RUN_MODE_LOW_SPEED)
        {
            /* Clear LPR bit to back the normal run mode */
            CLEAR_BIT(PWR->CR1, PWR_CR1_LPR);
            /* Enter STOP 2 mode  */
            HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
            /* Set Regulator parameter to lowpower run mode */
            SET_BIT(PWR->CR1, PWR_CR1_LPR);
        }
        else
        {
            /* Enter STOP 2 mode  */
            HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
        }
#else
        /* Enter STOP mode  */
        HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
#endif
        /* Enable SysTick interrupt */
        SET_BIT(SysTick->CTRL, (rt_uint32_t)SysTick_CTRL_TICKINT_Msk);
        /* Re-configure the system clock */
        //SystemClock_ReConfig(pm->run_mode);//fix frank
        break;

    case PM_SLEEP_MODE_STANDBY:
        /* Enter STANDBY mode */
        HAL_PWR_EnterSTANDBYMode();
        break;

    case PM_SLEEP_MODE_SHUTDOWN:
#if defined(SOC_SERIES_STM32L4)
        /* Enter SHUTDOWNN mode */
        HAL_PWREx_EnterSHUTDOWNMode();
#else
        /* Enter STANDBY mode */
        HAL_PWR_EnterSTANDBYMode();
#endif
        break;

    default:
        RT_ASSERT(0);
        break;
    }
}

RT_WEAK rt_uint8_t run_speed[PM_RUN_MODE_MAX][2] =
{
    {80, 0},
    {80, 1},
    {24, 2},
    {2,  3},
};

static void run(struct rt_pm *pm, rt_uint8_t mode)
{
    static rt_uint8_t last_mode;
    static char *run_str[] = PM_RUN_MODE_NAMES;

    if (mode == last_mode)
        return;
    last_mode = mode;

    /* 1. 设置 MSI �?HSI 作为 SYSCLK 时钟�?以修�?PLL */
   // SystemClock_MSI_ON();

    /* 2. 根据RUN模式切换时钟频率(HSI) */
    switch (mode)
    {
    case PM_RUN_MODE_HIGH_SPEED:
#if defined(PWR_REGULATOR_VOLTAGE_SCALE3) && !defined(STM32F401xx)
//        HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
//        SystemClock_RunModeHigh();  //fix frank
        break;
#endif
    case PM_RUN_MODE_NORMAL_SPEED:
#if defined(SOC_SERIES_STM32L0) || defined(SOC_SERIES_STM32L1) || defined(SOC_SERIES_STM32L4) || \
    defined(SOC_SERIES_STM32G0) || defined(SOC_SERIES_STM32G4)
        HAL_PWREx_DisableLowPowerRunMode();
        /* Configure the main internal regulator output voltage (Range1 by default)*/
       // HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
#elif defined(SOC_SERIES_STM32F4) || defined(SOC_SERIES_STM32F7) || defined(SOC_SERIES_STM32H7)
        HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE2);
#endif
        //SystemClock_RunModeNormal(); //fix frank
        break;
    case PM_RUN_MODE_MEDIUM_SPEED:
#if defined(SOC_SERIES_STM32L0) || defined(SOC_SERIES_STM32L1) || defined(SOC_SERIES_STM32L4) || \
    defined(SOC_SERIES_STM32G0) || defined(SOC_SERIES_STM32G4) || defined(SOC_SERIES_STM32H7) || \
    defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx)
#if !defined(SOC_SERIES_STM32F4) && !defined(SOC_SERIES_STM32H7)
        HAL_PWREx_DisableLowPowerRunMode();
#endif /* !defined(SOC_SERIES_STM32F4) */
       // SystemClock_RunModeMedium();
        /* Configure the main internal regulator output voltage */
#ifdef PWR_REGULATOR_VOLTAGE_SCALE3
       // HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE3);
#else
        HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE2);
#endif /* PWR_REGULATOR_VOLTAGE_SCALE3 */
#elif defined(SOC_SERIES_STM32F4) || defined(SOC_SERIES_STM32F7)
        /* Configure the main internal regulator output voltage */
#ifdef PWR_REGULATOR_VOLTAGE_SCALE3
        HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE3);
#else
        HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE2);
#endif /* PWR_REGULATOR_VOLTAGE_SCALE3 */
        SystemClock_RunModeMedium();
#else
        SystemClock_RunModeMedium();
#endif
        break;
    case PM_RUN_MODE_LOW_SPEED:
        //SystemClock_RunModeLow();//fix frank
#if defined(SOC_SERIES_STM32L0) || defined(SOC_SERIES_STM32L1) || defined(SOC_SERIES_STM32L4) || \
    defined(SOC_SERIES_STM32G0) || defined(SOC_SERIES_STM32G4)
        /* Enter LP RUN mode */
        HAL_PWREx_EnableLowPowerRunMode();
#endif
        break;
    default:
        break;
    }

    /* 3. 关闭 MSI 时钟 */
    // SystemClock_MSI_OFF();

    /* 4. 更新外设时钟 */
    uart_console_reconfig();
    /* Re-Configure the Systick time */
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / RT_TICK_PER_SECOND);
    /* Re-Configure the Systick */
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

    rt_kprintf("switch to %s mode, frequency = %d MHz\n", run_str[mode], run_speed[mode][0]);
}

/**
 * This function caculate the PM tick from OS tick
 *
 * @param tick OS tick
 *
 * @return the PM tick
 */
static rt_tick_t stm32_pm_tick_from_os_tick(rt_tick_t tick)
{
    rt_uint32_t freq = stm32_lptim_get_countfreq();

    return (freq * tick / RT_TICK_PER_SECOND);
}

/**
 * This function caculate the OS tick from PM tick
 *
 * @param tick PM tick
 *
 * @return the OS tick
 */
static rt_tick_t stm32_os_tick_from_pm_tick(rt_uint32_t tick)
{
    static rt_uint32_t os_tick_remain = 0;
    rt_uint32_t ret, freq;

    freq = stm32_lptim_get_countfreq();
    ret = (tick * RT_TICK_PER_SECOND + os_tick_remain) / freq;

    os_tick_remain += (tick * RT_TICK_PER_SECOND);
    os_tick_remain %= freq;

    return ret;
}

/**
 * This function start the timer of pm
 *
 * @param pm Pointer to power manage structure
 * @param timeout How many OS Ticks that MCU can sleep
 */
static void pm_timer_start(struct rt_pm *pm, rt_uint32_t timeout)
{
    RT_ASSERT(pm != RT_NULL);
    RT_ASSERT(timeout > 0);

    if (timeout != RT_TICK_MAX)
    {
        /* Convert OS Tick to pmtimer timeout value */
        timeout = stm32_pm_tick_from_os_tick(timeout);
        if (timeout > stm32_lptim_get_tick_max())
        {
            timeout = stm32_lptim_get_tick_max();
        }

        /* Enter PM_TIMER_MODE */
        stm32_lptim_start(timeout);
    }
}

/**
 * This function stop the timer of pm
 *
 * @param pm Pointer to power manage structure
 */
static void pm_timer_stop(struct rt_pm *pm)
{
    RT_ASSERT(pm != RT_NULL);

    /* Reset pmtimer status */
    stm32_lptim_stop();
}

/**
 * This function calculate how many OS Ticks that MCU have suspended
 *
 * @param pm Pointer to power manage structure
 *
 * @return OS Ticks
 */
static rt_tick_t pm_timer_get_tick(struct rt_pm *pm)
{
    rt_uint32_t timer_tick;

    RT_ASSERT(pm != RT_NULL);

    timer_tick = stm32_lptim_get_current_tick();

    return stm32_os_tick_from_pm_tick(timer_tick);
}

/**
 * This function initialize the power manager
 */
int drv_pm_hw_init(void)
{
    static const struct rt_pm_ops _ops =
    {
        sleep,
        run,
        pm_timer_start,
        pm_timer_stop,
        pm_timer_get_tick
    };

    rt_uint8_t timer_mask = 0;

    /* Enable Power Clock */
    __HAL_RCC_PWR_CLK_ENABLE();

    /* initialize timer mask */
    timer_mask = 1UL << PM_SLEEP_MODE_DEEP;

    /* initialize system pm module */
    rt_system_pm_init(&_ops, timer_mask, RT_NULL);

    return 0;
}

INIT_BOARD_EXPORT(drv_pm_hw_init);

#endif  /* RT_USING_PM */
