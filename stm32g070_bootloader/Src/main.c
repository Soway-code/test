/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "type.h"
#include "common.h"
#include "modbus_ascii.h"
#include "flash.h"

uint8_t OneFrameOk;                                      //���ݽ�����ɱ�־
uint8_t SlaveAddr;                                       //�ӻ���ַ
uint8_t UpgradeInfo;                                     //������Ϣ
uint8_t ProgErase;                                       //���������־
uint8_t UpgradeWaitTime;                                 //�����ȴ�ʱ��

pFunction Jump_To_Application;                           //��ת��Ӧ�ó���
uint32_t JumpAddress;                                    //��ת��ַ


uint8_t SendLen;                                                                //���ڷ��ͳ���
uint16_t PacketCnt;                                                             //�����
uint16_t PacketNum;                                                             //�ܰ���

uint32_t Flashadrdst;                                                           //FLASH��ַ
uint32_t FileCheckSum;                                                          //�����ļ�У���
uint32_t FileRunCheckSum;                                                       //�����ļ�ʵʱУ���

uint8_t SendBuf[50];                                                            //���ڷ�����ʱ����
extern uint8_t u8SendNum;
extern uint8_t UART1_RXBuff[USART1_MAX_DATALEN];//�������ݻ���
extern uint8_t UART1_TXBUFF[USART1_MAX_DATALEN];//�������ݻ���
FlashPara_Typedef	 FlashPara = {0};
void SystemClock_Config(void);
void HandleFrameInformation(void)
{
    uint8_t fmerror;                                                              //֡����
    uint8_t tmpaddr;                                                              //�ݴ��ַ
    uint16_t rcvlen;                                                              //�������ݳ���
    uint16_t prt;                                                                 //����
    uint16_t msgid;                                                               //��ϢID
    uint16_t datalen;                                                             //���ݳ���
    uint16_t packetnum;                                                           //�ܰ���
    uint16_t packetcnt;                                                           //�����
    uint32_t tpcksum;                                                             //��У���
    uint64_t *ramdata;                                                            //����ָ��

    fmerror = MODBUS_ASCII_RecvData(UART1_RXBuff, &rcvlen);                       //�������ݴ����򲻷���

    if((0x41 != UART1_RXBuff[1])||(0x00 == UART1_RXBuff[0]))                      //����������򲻷���
    {
        return ;
    }

    tmpaddr = UART1_RXBuff[0];                                                               //��ȡ�ӻ���ַ
    msgid = UART1_RXBuff[2]*256 + UART1_RXBuff[3];                                           //��ȡ��ϢID
    datalen = UART1_RXBuff[4]*256 + UART1_RXBuff[5];                                         //��ȡ���ݳ���
    packetnum = UART1_RXBuff[6]*256 + UART1_RXBuff[7];                                       //��ȡ�ܰ���
    packetcnt = UART1_RXBuff[8]*256 + UART1_RXBuff[9];                                       //��ȡ�����

    if((rcvlen - 0x06) != datalen)                                                          //���ݳ��Ȳ���ȷ�򲻷���
    {
        return ;
    }

    if((SlaveAddr != UART1_RXBuff[0])&&(0xFF != UART1_RXBuff[0]))                          //�ӻ���ַ��������Χ������
    {
        return ;
    }

    if((0 != UpgradeWaitTime)&&(0x0004 != msgid)&&(0x0001 != msgid))                         //����������Ϣ�йز�����
    {
        return ;
    }

    for(SendLen = 0; SendLen < 4; SendLen++)                                                 //�洢��������Ϣ
    {
        SendBuf[SendLen] = UART1_RXBuff[SendLen];
    }

    SendBuf[SendLen++] = 0x00;
    SendBuf[SendLen++] = 0x01;
    if(3 == fmerror)
    {
        switch(msgid)
        {
            case 0x0001:                                                        //��ʼ����
                UpgradeWaitTime = 0;
			    DIS_INT;
			    FlashReadNHalfWord(FLASH_BASE_ADDR, (uint16_t *)&FlashPara, sizeof(FlashPara)>>1);						// ��Flash��ǰ��������
                FlashPara.Common.UpgradeFlag=0xFF;
		        FlashPageErase(FLASH_BASE_ADDR);										// Flashд����֮ǰ �����Ȳ���
		        FlashWriteDoubleWord(FLASH_BASE_ADDR, (uint64_t *)&FlashPara, sizeof(FlashPara)>>3);       // ����ǰ����д��Flash��ǰ������	
		        EN_INT;																	// ���ж�ʧ�ܵ�ʹ�ܱ���Ӳ�����ʼ��д��ɹ�
                SendBuf[SendLen++] = 0x00;
                if((0 != datalen)&&(2 != datalen))
                {
                    SendBuf[SendLen - 1] = 0x01;
                    break;
                }
                prt = 0;
                PacketNum = 0;
                PacketCnt = 0;
                while(2048 > packetnum)//&&(packetnum))
                {
                    packetnum--;

//                    if(2046 != packetnum)
//                    {
//                        Eeprom_WriteByte(prt++, 0xFF);
//                    }
//                    else
//                    {
//                        if((0x0C == ProgErase) || (0xC0 == ProgErase))
//                        {
//                            Eeprom_WriteByte(2046, ProgErase);
//                        }
//                    }
					  if(2046 != packetnum)
					  {
//						    prt++;
							DIS_INT;
//							FlashReadNHalfWord(FLASH_BASE_ADDR, (uint16_t *)&FlashPara, sizeof(FlashPara)>>1);		// ��Flash��ǰ��������
//							FlashPara.Common.UpgradeFlag=0xFF;                                                              //��ȡ������Ϣ
//	                        SlaveAddr=FlashPara.Common.Addr=0xFF;
							FlashPageErase(FLASH_BASE_ADDR);										// Flashд����֮ǰ �����Ȳ���
//							FlashWriteDoubleWord(FLASH_BASE_ADDR, (uint64_t *)&FlashPara, sizeof(FlashPara)>>3);       // ����ǰ����д��Flash��ǰ������	
							EN_INT;
					  }
					  else
					  {
                         if((0x0C == ProgErase) || (0xC0 == ProgErase))
                         {
							DIS_INT;
							FlashReadNHalfWord(FLASH_BASE_ADDR, (uint16_t *)&FlashPara, sizeof(FlashPara)>>1);		// ��Flash��ǰ��������
							FlashPara.Common.ProgEraseFlag=ProgErase;
							FlashPageErase(FLASH_BASE_ADDR);										// Flashд����֮ǰ �����Ȳ���
							FlashWriteDoubleWord(FLASH_BASE_ADDR, (uint64_t *)&FlashPara, sizeof(FlashPara)>>3);       // ����ǰ����д��Flash��ǰ������	
							EN_INT;	
                         } 
					  }
                }
                break;

            case 0x0002:                                                        //���Դ����
                if(0 != datalen)
                {
                    SendBuf[SendLen++] = 0x01;
                    break;
                }
                ProgErase = 0x0C;
//                Eeprom_WriteByte(2046, ProgErase);
				DIS_INT;
				FlashReadNHalfWord(FLASH_BASE_ADDR, (uint16_t *)&FlashPara, sizeof(FlashPara)>>1);		// ��Flash��ǰ��������
				FlashPara.Common.ProgEraseFlag=ProgErase;
				FlashPageErase(FLASH_BASE_ADDR);										// Flashд����֮ǰ �����Ȳ���
				FlashWriteDoubleWord(FLASH_BASE_ADDR, (uint64_t *)&FlashPara, sizeof(FlashPara)>>3);       // ����ǰ����д��Flash��ǰ������	
				EN_INT;	
                DIS_INT;				
//                HAL_FLASH_Unlock();
                FLASH_If_Erase(APPLICATION_ADDRESS);  
                EN_INT;				
                Flashadrdst = APPLICATION_ADDRESS;
                SendBuf[SendLen++] = 0x00;
                break;

            case 0x0003:                                                        //���������ļ�
                if((0 == PacketNum) && (1 < packetnum) && (0 == packetcnt))
                {
                    FileCheckSum = 0;
                    FileRunCheckSum = 0;
                    PacketNum = packetnum;
                    PacketCnt = packetcnt;
                    SendBuf[SendLen++] = 0x00;

                    for(prt = 0; prt < 4; prt++)
                    {
                        FileCheckSum <<= 8;
                        FileCheckSum += UART1_RXBuff[10 + prt];
                    }
                }
                else if((PacketNum == packetnum) && (1 < packetnum) && (PacketCnt == (packetcnt - 1)) && (PacketNum > packetcnt))
                {
                    tpcksum = 0;
                    datalen = datalen - 4;

                    for(prt = 0; prt < datalen; prt++)
                    {
                    tpcksum += UART1_RXBuff[10 + prt];
                    }

                    Decoding(&UART1_RXBuff[10], datalen);

                    for(prt = 0; prt < datalen; prt++)
                    {
                        UART1_RXBuff[prt] = UART1_RXBuff[10 + prt];
                    }
                    ramdata = (uint64_t*)UART1_RXBuff;

                    if(FlashWriteDoubleWord(Flashadrdst, ramdata, datalen/8)  == 0)
                    {
						Flashadrdst+=datalen;
                        PacketCnt++;
                        SendBuf[SendLen++] = 0x00;
                        FileRunCheckSum += tpcksum;
                    }
                    else
                    {
                        SendBuf[SendLen++] = 0x01;
                    }
                }
                else if((PacketNum == packetnum)&&(1 < packetnum)&&(PacketCnt == packetcnt)&&(PacketNum > packetcnt))
                {
                    SendBuf[SendLen++] = 0x00;
                }
                else
                {
                    SendBuf[SendLen++] = 0x01;
                }
                break;

            case 0x0004:                                                                                  //ִ��Ӧ�ó���
                if(((FileRunCheckSum == FileCheckSum)&&((PacketCnt + 1) == PacketNum)||(0 == PacketNum))&&(0 == datalen))
                {
                    if(0 != PacketNum)
                    {
                        UpgradeWaitTime = 10;
                        SendBuf[SendLen++] = 0x00;
                    }
                    else if((0x0C == ProgErase)||(0xFF == ProgErase))
                    {
                        SendBuf[SendLen++] = 0x01;
                    }
                    else
                    {
                        UpgradeWaitTime = 10;
                        SendBuf[SendLen++] = 0x00;
                    }
                }
                else
                {
                    SendBuf[SendLen++] = 0x02;
                }
                break;

                default:
                    SendBuf[SendLen++] = 0x01;
                    break;
        }
    }
    else
    {
        SendBuf[SendLen++] = 0x02;                                                                     //У�����
    }
    if((0x00 != tmpaddr) && (SlaveAddr == tmpaddr) || (0xFF == tmpaddr))
    {
        if(0x0004 == msgid)
        {
            TimingDelay_SetTime(50);//�ӳ�Ӧ��
            while(!TimingDelay_TimeOut());
        }
        MODBUS_ASCII_SendData(SendBuf, SendLen);

		
    }
    if(0x0004 == msgid)
    {
        TimingDelay_SetTime(100);

        while(!TimingDelay_TimeOut());
    }
}

void JumpToUserApplication(void)
{
    SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
    
    /* Jump to user application */
    JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
    Jump_To_Application = (pFunction) JumpAddress;

    /* Initialize user application's Stack Pointer */
    __set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);

    /* Jump to application */
    Jump_To_Application();
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
//	uint8_t ch[] = "sagsg\r\n";
//    HAL_InitTick(3); 
	HAL_Init();
    SystemClock_Config();
    Uart_Config_Init();
    
	DIS_INT;
	FlashReadNHalfWord(FLASH_BASE_ADDR, (uint16_t *)&FlashPara, sizeof(FlashPara)>>3);						// ��Flash��ǰ��������
	EN_INT;
	UpgradeInfo=FlashPara.Common.UpgradeFlag;                                                              //��ȡ������Ϣ
	SlaveAddr=FlashPara.Common.Addr;
    ProgErase=FlashPara.Common.ProgEraseFlag;
    if((0x0C == UpgradeInfo) || (0xFF == UpgradeInfo) || (0x0C == ProgErase))   //������������������޵ȴ�
    {
        UpgradeWaitTime = 0;
    }
    else                                                                        //����ȴ�2��
    {
        UpgradeWaitTime = 2;
    }

    TimingDelay_SetTime(1000);

    while(1)
    {
        if(TimingDelay_TimeOut() && UpgradeWaitTime && (10 > UpgradeWaitTime))  //�ж��Ƿ����޵ȴ�
        {
            if(1 == UpgradeWaitTime)
            {
                break;
            }
            UpgradeWaitTime--;
            TimingDelay_SetTime(1000);
        }
        if(10 == UpgradeWaitTime)                                                   //�����������
        {
            break;
        }
        if(0x0C == OneFrameOk)                                                      //�������������                                                      
        {
            OneFrameOk = 0;
            HandleFrameInformation();                                                 //������յ�֡����
			
        }
    }
    if (((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
    {
        //uprintf("APP error!\r\n");
        
		DIS_INT;
		FlashReadNHalfWord(FLASH_BASE_ADDR, (uint16_t *)&FlashPara, sizeof(FlashPara)>>1);		// ��Flash��ǰ��������
        FlashPara.Common.UpgradeFlag=0x00;
		FlashPageErase(FLASH_BASE_ADDR);										// Flashд����֮ǰ �����Ȳ���
		FlashWriteDoubleWord(FLASH_BASE_ADDR, (uint64_t *)&FlashPara, sizeof(FlashPara)>>3);       // ����ǰ����д��Flash��ǰ������	
		EN_INT;																	// ���ж�ʧ�ܵ�ʹ�ܱ���Ӳ�����ʼ��д��ɹ�
        
		DIS_INT;
		FlashReadNHalfWord(FLASH_BASE_ADDR, (uint16_t *)&FlashPara, sizeof(FlashPara)>>1);		// ��Flash��ǰ��������
        FlashPara.Common.ProgEraseFlag=0xC0;
		FlashPageErase(FLASH_BASE_ADDR);										// Flashд����֮ǰ �����Ȳ���
		FlashWriteDoubleWord(FLASH_BASE_ADDR, (uint64_t *)&FlashPara, sizeof(FlashPara)>>3);       // ����ǰ����д��Flash��ǰ������	
		EN_INT;																	// ���ж�ʧ�ܵ�ʹ�ܱ���Ӳ�����ʼ��д��ɹ�
        JumpToUserApplication();                                                    //ִ��Ӧ�ó���
    }
    while(1)
    {
//		UART1_SendData(ch, sizeof(ch));
//		HAL_Delay(1000);
        //uprintf("No APP!\r\n");
    }
}





void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage 
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks 
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_ADC;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}




/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
