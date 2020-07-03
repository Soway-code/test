#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "type.h"
#include "usart.h"
#include "modbus_ascii.h"
#include "main.h"

uint8_t u8SendNum = 0;                  //当前发送数据个数
uint8_t u8SendIndex = 0;                //发送数据缓冲前缀
uint8_t UART1_RXBuffLen = 0;            //接收数据缓存长度

BitAction UartRecvNewData = Bit_RESET;  //接收新数据标识
BitAction UartRecvFrameOK = Bit_RESET;  //接收数据完成标志

uint8_t UART1_RXBuff[USART1_MAX_DATALEN];//接收数据缓存
uint8_t UART1_TXBUFF[USART1_MAX_DATALEN];//发送数据缓存

UART_HandleTypeDef huart1;    //串口参数  

//****************************************************************************************************************************************************************
// 名称               : En485_Gpio_Init()
// 创建日期           : 2017-11-27
// 作者               : 梅梦醒
// 功能               : RS485使能引脚配置
// 输入参数           : 无
// 输出参数           : 无
// 返回结果           : 无
// 注意和说明         : 
// 修改内容           :
//****************************************************************************************************************************************************************
void En485_Gpio_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_GPIOB_CLK_ENABLE();                                               //使能GPIOA时钟  
    GPIO_InitStructure.Pin = EN_485_PIN;                                        
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(EN_485_PORT, &GPIO_InitStructure);                            
    TX_OFF;                                                                     //初始化为接收状态
}



//****************************************************************************************************************************************************************
// 名称               : Usart_Gpio_Init()
// 创建日期           : 2017-11-27
// 作者               : 梅梦醒
// 功能               : 通用串口收发引脚配置
// 输入参数           : 无
// 输出参数           : 无
// 返回结果           : 无
// 注意和说明         : 无
// 修改内容           :
//****************************************************************************************************************************************************************
void Usart_Gpio_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;  
     __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStructure.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStructure.Alternate = GPIO_AF1_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
}




//****************************************************************************************************************************************************************
// 名称               : Usart_Config_Init()
// 创建日期           : 2017-11-27
// 作者               : 梅梦醒
// 功能               : 通用串口参数初始化
// 输入参数           : UARTx_ConfigTypeDef(串口通信参数)
// 输出参数           : 无
// 返回结果           : 无
// 注意和说明         : 无
// 修改内容           :
//****************************************************************************************************************************************************************
void Uart_Config_Init(void)
{
    __HAL_RCC_USART1_CLK_ENABLE();
    
    En485_Gpio_Init();                  //485使能脚配置
    Usart_Gpio_Init();                  //USART管脚配置
    
    HAL_NVIC_SetPriority(USART1_IRQn, 1, 1);//优先级
    HAL_NVIC_EnableIRQ(USART1_IRQn);        //使能总中断
    
	huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    huart1.FifoMode = UART_FIFOMODE_DISABLE;
    HAL_UART_Init(&huart1);
      
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);                     //使能接收中断
    __HAL_UART_ENABLE(&huart1);                                      //使能UART1
}





//******************************************************************************
// 名称               : UART1_RecvData()
// 创建日期           : 2017-11-27
// 作者               : 梅梦醒
// 功能               : UART1接收数据函数
// 输入参数           : uint8_t *UART1_RecvBuff 接收数据缓存
//                      uint32_t Len            接收数据长度        
// 输出参数           : 无
// 返回结果           : 无
// 注意和说明         : 
// 修改内容           :
//******************************************************************************
int32_t UART1_RecvData(uint8_t *UART1_RecvBuff, uint32_t Len)
{
    uint32_t i = 0;

    if ((0 == Len) || (((uint8_t*)NULL) == UART1_RecvBuff))
    {
        return 0;
    }

    if ((Bit_RESET == UartRecvFrameOK) || (0 == UART1_RXBuffLen))
    {
        return 0;
    }

    if (Len < UART1_RXBuffLen)
    {
        return -1;
    }

    Len = UART1_RXBuffLen;

    for (i = 0; i < Len; i++)
    {
        UART1_RecvBuff[i] = UART1_RXBuff[i];
    }

    UartRecvFrameOK = Bit_RESET;

    UART1_RXBuffLen = 0;

    return Len;
}


//******************************************************************************
// 名称               : UART1_SendData()
// 创建日期           : 2017-11-27
// 作者               : 梅梦醒
// 功能               : UART1发送数据函数
// 输入参数           : uint8_t *UART1_SendBuff 发送数据缓存
//                      uint32_t Len            发送数据长度        
// 输出参数           : 无
// 返回结果           : 无
// 注意和说明         : 
// 修改内容           :
//******************************************************************************
uint32_t UART1_SendData(uint8_t *UART1_SendBuff, uint32_t Len)
{
    uint32_t i = 0;

    if ((0 == Len) || (((uint8_t*)0) == UART1_SendBuff))
        return 0;

    if (u8SendNum != 0)
    {
        return 0;
    }

    if (Len > (sizeof(UART1_TXBUFF) / sizeof(UART1_TXBUFF[0])))
    {
        Len = (sizeof(UART1_TXBUFF) / sizeof(UART1_TXBUFF[0]));
    }

    for (i = 0; i < Len; i++)
    {
        UART1_TXBUFF[i] = UART1_SendBuff[i];
    }
    TX_ON;
    USART1->TDR = UART1_TXBUFF[0];
    u8SendIndex = 1;
    u8SendNum = Len;
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_TC);

    return Len;
}



//******************************************************************************
// 名称               : USART1_IRQHandler()
// 创建日期           : 2017-11-27
// 作者               : 梅梦醒
// 功能               : UART1发送中断
// 输入参数           : 无
//                      无       
// 输出参数           : 无
// 返回结果           : 无
// 注意和说明         : 
// 修改内容           :
//******************************************************************************
void USART1_IRQHandler(void)
{
    uint8_t RecvByteData;
  
    if(__HAL_UART_GET_IT(&huart1, UART_IT_RXNE) != RESET)
    {
        __HAL_UART_CLEAR_FLAG(&huart1, UART_IT_RXNE);
        
        RecvByteData = USART1->RDR;

        MODBUS_ASCII_HandlRevData(RecvByteData); //接收ASCII数据     
    }
    
    if (__HAL_UART_GET_IT(&huart1, UART_IT_TC) != RESET)
    {
        __HAL_UART_CLEAR_FLAG(&huart1, UART_IT_TC);

        if (u8SendIndex >= u8SendNum)
        {
            u8SendNum = 0;
            __HAL_UART_DISABLE_IT(&huart1, UART_IT_TC);
            TX_OFF;
        }
        else
        {
            USART1->TDR = UART1_TXBUFF[u8SendIndex++];
        }
    }
	HAL_UART_IRQHandler(&huart1);
}


//**************************************************************************************************
// 名称         : uprintf(const char *fmt,...)
// 创建日期     : 2016-09-08
// 作者         : 梅梦醒
// 功能         : 向串口发送指定格式的数据
// 输入参数     : 入：fmt,...    发送数据指针
// 输出参数     : 无
// 返回结果     : 无
// 注意和说明   : 用usart打印串口的数据，用于调试
// 修改内容     : 
//**************************************************************************************************
void uprintf(const char *fmt,...)
{
    va_list marker;
    char buff[64];
    memset(buff,0,sizeof(buff));
    va_start(marker, fmt);
    vsprintf(buff,fmt,marker);
    va_end(marker);
    UART1_SendData((uint8_t*)buff, strlen(buff));
    while(u8SendNum!=0);    
}


