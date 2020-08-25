#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Configuration */

/* RT-Thread Kernel */

#define RT_NAME_MAX 8
#define RT_ALIGN_SIZE 4
#define RT_THREAD_PRIORITY_32
#define RT_THREAD_PRIORITY_MAX 32
#define RT_TICK_PER_SECOND 100
#define RT_USING_OVERFLOW_CHECK
#define RT_USING_HOOK
#define RT_USING_IDLE_HOOK
#define RT_IDEL_HOOK_LIST_SIZE 4
#define IDLE_THREAD_STACK_SIZE 256
#define RT_DEBUG
#define RT_DEBUG_COLOR

/* Inter-Thread communication */

#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX
#define RT_USING_EVENT
#define RT_USING_MAILBOX
#define RT_USING_MESSAGEQUEUE

/* Memory Management */

#define RT_USING_MEMPOOL
#define RT_USING_SMALL_MEM
#define RT_USING_HEAP

/* Kernel Device Object */

#define RT_USING_DEVICE
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 128
#define RT_CONSOLE_DEVICE_NAME "uart1"
#define RT_VER_NUM 0x30103
#define ARCH_ARM
#define ARCH_ARM_CORTEX_M
#define ARCH_ARM_CORTEX_M0

/* RT-Thread Components */

#define RT_USING_COMPONENTS_INIT
#define RT_USING_USER_MAIN
#define RT_MAIN_THREAD_STACK_SIZE 2048
#define RT_MAIN_THREAD_PRIORITY 10

/* C++ features */


/* Command shell */

#define RT_USING_FINSH
#define FINSH_THREAD_NAME "tshell"
#define FINSH_USING_HISTORY
#define FINSH_HISTORY_LINES 5
#define FINSH_USING_SYMTAB
#define FINSH_USING_DESCRIPTION
#define FINSH_THREAD_PRIORITY 20
#define FINSH_THREAD_STACK_SIZE 4096
#define FINSH_CMD_SIZE 80
#define FINSH_USING_MSH
#define FINSH_USING_MSH_DEFAULT
#define FINSH_ARG_MAX 10

/* Device virtual file system */


/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_PIPE_BUFSZ 512
#define RT_USING_SERIAL
#define RT_SERIAL_RB_BUFSZ 64
#define RT_USING_HWTIMER
#define RT_USING_I2C
#define RT_USING_I2C_BITOPS
#define RT_USING_PIN
#define RT_USING_SENSOR
#define RT_USING_SENSOR_CMD

/* Using WiFi */


/* Using USB */


/* POSIX layer and C standard library */


/* Network */

/* Socket abstraction layer */


/* Network interface device */


/* light weight TCP/IP stack */


/* Modbus master and slave stack */


/* AT commands */


/* VBUS(Virtual Software BUS) */


/* Utilities */


/* RT-Thread online packages */

/* IoT - internet of things */

#define PKG_USING_FREEMODBUS
#define PKG_MODBUS_SLAVE

/* advanced configuration */

#define RT_S_DISCRETE_INPUT_START 0
#define RT_S_DISCRETE_INPUT_NDISCRETES 16
#define RT_S_COIL_START 0
#define RT_S_COIL_NCOILS 64
#define RT_S_REG_INPUT_START 0
#define RT_S_REG_INPUT_NREGS 100
#define RT_S_REG_HOLDING_START 0
#define RT_S_REG_HOLDING_NREGS 100
#define RT_S_HD_RESERVE 0
#define RT_S_IN_RESERVE 0
#define RT_S_CO_RESERVE 0
#define RT_S_DI_RESERVE 0
#define PKG_MODBUS_SLAVE_ASCII
#define PKG_MODBUS_SLAVE_SAMPLE
#define MB_SAMPLE_SLAVE_ADDR 1
#define MB_SLAVE_USING_PORT_NUM 1
#define MB_SLAVE_USING_PORT_BAUDRATE 115200
#define PKG_USING_FREEMODBUS_LATEST_VERSION

/* Wi-Fi */

/* Marvell WiFi */


/* Wiced WiFi */


/* IoT Cloud */


/* security packages */


/* language packages */


/* multimedia packages */


/* tools packages */


/* system packages */


/* peripheral libraries and drivers */

#define PKG_USING_SENSORS_DRIVERS
#define PKG_USING_U8G2
#define U8G2_USE_HW_I2C
#define U8G2_I2C_DEVICE_NAME "i2c2"

/* U8G2 Examples */

#define U8G2_USING_SW_I2C_SSD1306
#define U8G2_USING_HW_I2C_SSD1306

/* full buffer examples    (fast, large RAM consumption) */


/* page buffer examples    (slow, small RAM consumption) */


/* u8x8 text only examples (fonts with fixed size)       */


/* game examples */

#define PKG_USING_U8G2_CPP_LATEST_VERSION

/* C   version: v1.x.x */

/* C++ version: v2.x.x */

#define PKG_U8G2_VER_NUM 0x29999

/* miscellaneous packages */


/* samples: kernel and components samples */

#define SOC_FAMILY_STM32
#define SOC_SERIES_STM32G0

/* Hardware Drivers Config */

#define SOC_STM32G071RB

/* Onboard Peripheral Drivers */

#define BSP_USING_USB_TO_USART

/* On-chip Peripheral Drivers */

#define BSP_USING_GPIO
#define BSP_USING_UART
#define BSP_USING_LPUART1
#define BSP_USING_UART1
#define BSP_USING_TIM
#define BSP_USING_TIM3
#define BSP_USING_I2C1
#define BSP_I2C1_SCL_PIN 22
#define BSP_I2C1_SDA_PIN 23

/* Board extended module Drivers */


#endif
