#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "type.h"
#include "usart.h"
#include "modbus_ascii.h"
#include "main.h"

uint8_t u8SendNum = 0;                  //��ǰ�������ݸ���
uint8_t u8SendIndex = 0;                //�������ݻ���ǰ׺
uint8_t UART1_RXBuffLen = 0;            //�������ݻ��泤��

BitAction UartRecvNewData = Bit_RESET;  //���������ݱ�ʶ
BitAction UartRecvFrameOK = Bit_RESET;  //����������ɱ�־

uint8_t UART1_RXBuff[USART1_MAX_DATALEN];//�������ݻ���
uint8_t UART1_TXBUFF[USART1_MAX_DATALEN];//�������ݻ���

UART_HandleTypeDef huart1;    //���ڲ���  

//****************************************************************************************************************************************************************
// ����               : En485_Gpio_Init()
// ��������           : 2017-11-27
// ����               : ÷����
// ����               : RS485ʹ����������
// �������           : ��
// �������           : ��
// ���ؽ��           : ��
// ע���˵��         : 
// �޸�����           :
//****************************************************************************************************************************************************************
void En485_Gpio_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_GPIOB_CLK_ENABLE();                                               //ʹ��GPIOAʱ��  
    GPIO_InitStructure.Pin = EN_485_PIN;                                        
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(EN_485_PORT, &GPIO_InitStructure);                            
    TX_OFF;                                                                     //��ʼ��Ϊ����״̬
}



//****************************************************************************************************************************************************************
// ����               : Usart_Gpio_Init()
// ��������           : 2017-11-27
// ����               : ÷����
// ����               : ͨ�ô����շ���������
// �������           : ��
// �������           : ��
// ���ؽ��           : ��
// ע���˵��         : ��
// �޸�����           :
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
// ����               : Usart_Config_Init()
// ��������           : 2017-11-27
// ����               : ÷����
// ����               : ͨ�ô��ڲ�����ʼ��
// �������           : UARTx_ConfigTypeDef(����ͨ�Ų���)
// �������           : ��
// ���ؽ��           : ��
// ע���˵��         : ��
// �޸�����           :
//****************************************************************************************************************************************************************
void Uart_Config_Init(void)
{
    __HAL_RCC_USART1_CLK_ENABLE();
    
    En485_Gpio_Init();                  //485ʹ�ܽ�����
    Usart_Gpio_Init();                  //USART�ܽ�����
    
    HAL_NVIC_SetPriority(USART1_IRQn, 1, 1);//���ȼ�
    HAL_NVIC_EnableIRQ(USART1_IRQn);        //ʹ�����ж�
    
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
      
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);                     //ʹ�ܽ����ж�
    __HAL_UART_ENABLE(&huart1);                                      //ʹ��UART1
}





//******************************************************************************
// ����               : UART1_RecvData()
// ��������           : 2017-11-27
// ����               : ÷����
// ����               : UART1�������ݺ���
// �������           : uint8_t *UART1_RecvBuff �������ݻ���
//                      uint32_t Len            �������ݳ���        
// �������           : ��
// ���ؽ��           : ��
// ע���˵��         : 
// �޸�����           :
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
// ����               : UART1_SendData()
// ��������           : 2017-11-27
// ����               : ÷����
// ����               : UART1�������ݺ���
// �������           : uint8_t *UART1_SendBuff �������ݻ���
//                      uint32_t Len            �������ݳ���        
// �������           : ��
// ���ؽ��           : ��
// ע���˵��         : 
// �޸�����           :
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
// ����               : USART1_IRQHandler()
// ��������           : 2017-11-27
// ����               : ÷����
// ����               : UART1�����ж�
// �������           : ��
//                      ��       
// �������           : ��
// ���ؽ��           : ��
// ע���˵��         : 
// �޸�����           :
//******************************************************************************
void USART1_IRQHandler(void)
{
    uint8_t RecvByteData;
  
    if(__HAL_UART_GET_IT(&huart1, UART_IT_RXNE) != RESET)
    {
        __HAL_UART_CLEAR_FLAG(&huart1, UART_IT_RXNE);
        
        RecvByteData = USART1->RDR;

        MODBUS_ASCII_HandlRevData(RecvByteData); //����ASCII����     
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
// ����         : uprintf(const char *fmt,...)
// ��������     : 2016-09-08
// ����         : ÷����
// ����         : �򴮿ڷ���ָ����ʽ������
// �������     : �룺fmt,...    ��������ָ��
// �������     : ��
// ���ؽ��     : ��
// ע���˵��   : ��usart��ӡ���ڵ����ݣ����ڵ���
// �޸�����     : 
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


