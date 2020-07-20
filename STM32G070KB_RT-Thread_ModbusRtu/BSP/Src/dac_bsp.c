/**@file        dac_bsp.c
* @brief        DAC驱动
* @details      DAC 通道配置, 传输配置, DAC转换
* @author       杨春林
* @date         2020-04-27
* @version      V1.0.0
* @copyright    2020-2030,深圳市信为科技发展有限公司
**********************************************************************************
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/04/27  <td>1.0.0    <td>杨春林    <td>创建初始版本
* </table>
*
**********************************************************************************
*/

#include "dac_bsp.h"
#include "VariaType.h"


static DAC_HandleTypeDef hdac;      ///< DAC处理对象


/**@brief       DAC初始化
* @return       函数执行结果
* - None
* @note         DAC通道通过DAC_CHANNEL_1_ENABLE和DAC_CHANNEL_2_ENABLE宏定义打开
*/
void BSP_DAC_Init(void)
{
#if defined(DAC_CHANNEL_1_ENABLE) || defined(DAC_CHANNEL_2_ENABLE)
    
  DAC_ChannelConfTypeDef sConfig = {0};

  hdac.Instance = DACx;
  if (HAL_DAC_Init(&hdac) != HAL_OK)
  {
    Error_Handler();
  }
  
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
#if defined(DAC_CHANNEL_1_ENABLE) 
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
#endif
  
#if defined(DAC_CHANNEL_2_ENABLE)
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
#endif
  
#endif
}

/**@brief       DAC时钟、底层管脚初始化(由HAL_DAC_Init回调调用)
* @param[in]    dacHandle : DAC处理对象
* @return       函数执行结果
* - None
* @note         DAC通道通过DAC_CHANNEL_1_ENABLE和DAC_CHANNEL_2_ENABLE宏定义打开
*/
void HAL_DAC_MspInit(DAC_HandleTypeDef* dacHandle)
{
#if defined(DAC_CHANNEL_1_ENABLE) || defined(DAC_CHANNEL_2_ENABLE)

    GPIO_InitTypeDef GPIO_InitStruct = {0};


    DACx_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
#if defined(DAC_CHANNEL_1_ENABLE)
    GPIO_InitStruct.Pin |= DACx_CHANNEL_PIN;
    HAL_GPIO_Init(DACx_CHANNEL1_GPIO_PORT, &GPIO_InitStruct);
#endif
#if defined(DAC_CHANNEL_2_ENABLE)
    GPIO_InitStruct.Pin |= DACx_CHANNEL2_PIN;
    HAL_GPIO_Init(DACx_CHANNEL2_GPIO_PORT, &GPIO_InitStruct);
#endif
    

    /* USER CODE BEGIN DAC_MspInit 1 */

    /* USER CODE END DAC_MspInit 1 */
  
#endif
}

/**@brief       DAC时钟、底层管脚反初始化(由HAL_DAC_DeInit回调调用)
* @param[in]    dacHandle : DAC处理对象
* @return       函数执行结果
* - None
* @note         DAC通道通过DAC_CHANNEL_1_ENABLE和DAC_CHANNEL_2_ENABLE宏定义打开
*/
void HAL_DAC_MspDeInit(DAC_HandleTypeDef* dacHandle)
{
#if defined(DAC_CHANNEL_1_ENABLE) || defined(DAC_CHANNEL_2_ENABLE)

    /* USER CODE BEGIN DAC_MspDeInit 0 */

    /* USER CODE END DAC_MspDeInit 0 */
    /* Peripheral clock disable */
    DACx_CLK_DISABLE();

#if defined(DAC_CHANNEL_1_ENABLE)
    HAL_GPIO_DeInit(DACx_CHANNEL1_GPIO_PORT, DACx_CHANNEL1_PIN);
#endif
#if defined(DAC_CHANNEL_2_ENABLE)
    HAL_GPIO_DeInit(DACx_CHANNEL2_GPIO_PORT, DACx_CHANNEL2_PIN);
#endif


    /* USER CODE BEGIN DAC_MspDeInit 1 */

    /* USER CODE END DAC_MspDeInit 1 */
  
#endif
} 

/**@brief       指定DAC通道写入DA值
* @param[in]    DA_Value : DAC值
* @param[in]    DA_Channel_Num : DAC通道号
* @return       函数执行结果
* - None
* @note         DA值不能超过4095; 通道号见DA_CHANNEL_NUM的定义,
* 其值只能取DA_CHANNEL_NUM定义的值
*/
uint8_t BSP_DAC_Convert(uint16_t DA_Value, DA_CHANNEL_NUM DA_Channel_Num)
{
  uint32_t channel;
  //DAC值超出范围
  if(DA_Value > 4095)
  {
    return OP_FAILED;
  }

  if(DA_Channel_Num != DA_CHANNEL_1 && DA_Channel_Num != DA_CHANNEL_2)
  {
    return OP_FAILED;
  }
  
  if(DA_Channel_Num == DA_CHANNEL_1)
      channel = DAC_CHANNEL_1;
  else if(DA_Channel_Num == DA_CHANNEL_2)
      channel = DAC_CHANNEL_2;
  
  HAL_DAC_SetValue(&hdac, channel, DAC_ALIGN_12B_R, DA_Value);
  HAL_DAC_Start(&hdac, channel);
  
  return OP_SUCCESS;
}

/* 使用RT-Thread操作系统,USING_RT_THREAD_OS在main.h中定义 */
#ifdef USING_RT_THREAD_OS

static struct rt_device dac_device_obj;     //DAC设备对象


/**@brief       DAC设备初始化,本函数被rt_device_register注册后,
* 被rt_device_init或rt_device_open调用
* @param[in]    dev : 设备句柄
* @return       函数执行结果
* - RT_EOK: 设备初始化成功; 
*/
static rt_err_t stm32_dac_init(rt_device_t dev)
{
    BSP_DAC_Init();
    
    return RT_EOK;
}

/**@brief       DAC设备打开,本函数被rt_device_register注册后,
* 被rt_device_open调用
* @param[in]    dev : 设备句柄
* @param[in]    oflag : 设备访问模式标志
* @return       函数执行结果
* - RT_EOK: 设备打开成功; 
* @note         这里不使用设备打开,故只返回一个RT_EOK
*/
static rt_err_t  stm32_dac_open(rt_device_t dev, rt_uint16_t oflag)
{
    return RT_EOK;
}

/**@brief       DAC设备关闭,本函数被rt_device_register注册后,
* 被rt_device_close调用
* @param[in]    dev : 设备句柄
* @param[in]    oflag : 设备访问模式标志
* @return       函数执行结果
* - RT_EOK: 设备关闭成功; 
* @note         这里不使用设备关闭,故只返回一个RT_EOK
*/
static rt_err_t  stm32_dac_close(rt_device_t dev)
{
    return RT_EOK;
}

/**@brief       DAC设备写数据,本函数被rt_device_register注册后,
* 被rt_device_write调用
* @param[in]    dev : 设备句柄
* @param[in]    pos : 写入数据偏移量;
* @param[in]    buffer : 内存缓冲区指针，写入的数据将会被保存在缓冲区中;
* @param[in]    size : 写入数据的大小
* @return       函数执行结果
* - size: 写入数据的实际大小，如果是字符设备，返回大小以字节为
* 单位，如果是块设备，返回的大小以块为单位;
* - 0: 需要读取当前线程的 errno 来判断错误状态
*/
static rt_size_t stm32_dac_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    uint8_t i;
    
    for(i = pos; i < (size + pos); i++)
    {
        if(i >= DA_CHANNEL_TOTAL)
            return i - pos;
        BSP_DAC_Convert(*(uint16_t *)buffer, (DA_CHANNEL_NUM)i);
    }
    return size;
}

/**@brief       DAC设备控制,本函数被rt_device_register注册后,被
* rt_device_control调用
* @param[in]    dev : 设备句柄
* @param[in]    cmd : 命令控制字，这个参数通常与设备驱动程序相关;
* @param[in]    arg : 控制的参数
* @return       函数执行结果
* - RT_EOK:函数执行成功;
* @note         这里不使用设备控制,故只返回一个RT_EOK
*/
static rt_err_t  stm32_dac_control(rt_device_t dev, int cmd, void *args)
{    
    return RT_EOK;
}

/**@brief       DAC设备参数初始化,注册ADC设备
* @return       函数执行结果
* - int整型值(RT_EOK)
* @note         本函数使用RT-Thread的自动初始化组件INIT_DEVICE_EXPORT
* 调用执行,系统复位后自动初始化
*/
static int dac_device_init(void)
{
    dac_device_obj.type     = RT_Device_Class_Miscellaneous;
    dac_device_obj.init     = stm32_dac_init;
    dac_device_obj.open     = stm32_dac_open;
    dac_device_obj.close    = stm32_dac_close;
    dac_device_obj.read     = RT_NULL;
    dac_device_obj.write    = stm32_dac_write;
    dac_device_obj.control  = stm32_dac_control;
    
    dac_device_obj.user_data = &dac_device_obj;
    
    rt_device_register(&dac_device_obj, DAC_DEVICE_NAME, 
                        RT_DEVICE_FLAG_WRONLY
                        | RT_DEVICE_FLAG_STANDALONE);
    
    return RT_EOK;
}
INIT_DEVICE_EXPORT(dac_device_init);

#endif

