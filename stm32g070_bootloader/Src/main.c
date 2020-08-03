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

uint8_t OneFrameOk;                                      //数据接收完成标志
uint8_t SlaveAddr;                                       //从机地址
uint8_t UpgradeInfo;                                     //升级信息
uint8_t ProgErase;                                       //程序擦除标志
uint8_t UpgradeWaitTime;                                 //升级等待时间

pFunction Jump_To_Application;                           //跳转到应用程序
uint32_t JumpAddress;                                    //跳转地址


uint8_t SendLen;                                                                //串口发送长度
uint16_t PacketCnt;                                                             //包序号
uint16_t PacketNum;                                                             //总包数

uint32_t Flashadrdst;                                                           //FLASH地址
uint32_t FileCheckSum;                                                          //升级文件校验和
uint32_t FileRunCheckSum;                                                       //升级文件实时校验和

uint8_t SendBuf[50];                                                            //串口发送临时缓存
extern uint8_t u8SendNum;
extern uint8_t UART1_RXBuff[USART1_MAX_DATALEN];//接收数据缓存
extern uint8_t UART1_TXBUFF[USART1_MAX_DATALEN];//发送数据缓存
FlashPara_Typedef	 FlashPara = {0};
void SystemClock_Config(void);
void HandleFrameInformation(void)
{
    uint8_t fmerror;                                                              //帧错误
    uint8_t tmpaddr;                                                              //暂存地址
    uint16_t rcvlen;                                                              //接收数据长度
    uint16_t prt;                                                                 //计数
    uint16_t msgid;                                                               //消息ID
    uint16_t datalen;                                                             //数据长度
    uint16_t packetnum;                                                           //总包数
    uint16_t packetcnt;                                                           //包序号
    uint32_t tpcksum;                                                             //包校验和
    uint64_t *ramdata;                                                            //数据指针

    fmerror = MODBUS_ASCII_RecvData(UART1_RXBuff, &rcvlen);                       //接收数据错误则不返回

    if((0x41 != UART1_RXBuff[1])||(0x00 == UART1_RXBuff[0]))                      //功能码错误则不返回
    {
        return ;
    }

    tmpaddr = UART1_RXBuff[0];                                                               //获取从机地址
    msgid = UART1_RXBuff[2]*256 + UART1_RXBuff[3];                                           //获取消息ID
    datalen = UART1_RXBuff[4]*256 + UART1_RXBuff[5];                                         //获取数据长度
    packetnum = UART1_RXBuff[6]*256 + UART1_RXBuff[7];                                       //获取总包数
    packetcnt = UART1_RXBuff[8]*256 + UART1_RXBuff[9];                                       //获取包序号

    if((rcvlen - 0x06) != datalen)                                                          //数据长度不正确则不返回
    {
        return ;
    }

    if((SlaveAddr != UART1_RXBuff[0])&&(0xFF != UART1_RXBuff[0]))                          //从机地址不在允许范围不处理
    {
        return ;
    }

    if((0 != UpgradeWaitTime)&&(0x0004 != msgid)&&(0x0001 != msgid))                         //不是升级信息有关不处理
    {
        return ;
    }

    for(SendLen = 0; SendLen < 4; SendLen++)                                                 //存储待发送信息
    {
        SendBuf[SendLen] = UART1_RXBuff[SendLen];
    }

    SendBuf[SendLen++] = 0x00;
    SendBuf[SendLen++] = 0x01;
    if(3 == fmerror)
    {
        switch(msgid)
        {
            case 0x0001:                                                        //开始升级
                UpgradeWaitTime = 0;
			    DIS_INT;
			    FlashReadNHalfWord(FLASH_BASE_ADDR, (uint16_t *)&FlashPara, sizeof(FlashPara)>>1);						// 将Flash当前参数读出
                FlashPara.Common.UpgradeFlag=0xFF;
		        FlashPageErase(FLASH_BASE_ADDR);										// Flash写数据之前 必须先擦除
		        FlashWriteDoubleWord(FLASH_BASE_ADDR, (uint64_t *)&FlashPara, sizeof(FlashPara)>>3);       // 将当前数据写入Flash当前参数区	
		        EN_INT;																	// 总中断失能到使能必须从擦除开始到写入成功
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
//							FlashReadNHalfWord(FLASH_BASE_ADDR, (uint16_t *)&FlashPara, sizeof(FlashPara)>>1);		// 将Flash当前参数读出
//							FlashPara.Common.UpgradeFlag=0xFF;                                                              //读取升级信息
//	                        SlaveAddr=FlashPara.Common.Addr=0xFF;
							FlashPageErase(FLASH_BASE_ADDR);										// Flash写数据之前 必须先擦除
//							FlashWriteDoubleWord(FLASH_BASE_ADDR, (uint64_t *)&FlashPara, sizeof(FlashPara)>>3);       // 将当前数据写入Flash当前参数区	
							EN_INT;
					  }
					  else
					  {
                         if((0x0C == ProgErase) || (0xC0 == ProgErase))
                         {
							DIS_INT;
							FlashReadNHalfWord(FLASH_BASE_ADDR, (uint16_t *)&FlashPara, sizeof(FlashPara)>>1);		// 将Flash当前参数读出
							FlashPara.Common.ProgEraseFlag=ProgErase;
							FlashPageErase(FLASH_BASE_ADDR);										// Flash写数据之前 必须先擦除
							FlashWriteDoubleWord(FLASH_BASE_ADDR, (uint64_t *)&FlashPara, sizeof(FlashPara)>>3);       // 将当前数据写入Flash当前参数区	
							EN_INT;	
                         } 
					  }
                }
                break;

            case 0x0002:                                                        //清除源程序
                if(0 != datalen)
                {
                    SendBuf[SendLen++] = 0x01;
                    break;
                }
                ProgErase = 0x0C;
//                Eeprom_WriteByte(2046, ProgErase);
				DIS_INT;
				FlashReadNHalfWord(FLASH_BASE_ADDR, (uint16_t *)&FlashPara, sizeof(FlashPara)>>1);		// 将Flash当前参数读出
				FlashPara.Common.ProgEraseFlag=ProgErase;
				FlashPageErase(FLASH_BASE_ADDR);										// Flash写数据之前 必须先擦除
				FlashWriteDoubleWord(FLASH_BASE_ADDR, (uint64_t *)&FlashPara, sizeof(FlashPara)>>3);       // 将当前数据写入Flash当前参数区	
				EN_INT;	
                DIS_INT;				
//                HAL_FLASH_Unlock();
                FLASH_If_Erase(APPLICATION_ADDRESS);  
                EN_INT;				
                Flashadrdst = APPLICATION_ADDRESS;
                SendBuf[SendLen++] = 0x00;
                break;

            case 0x0003:                                                        //传输升级文件
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

            case 0x0004:                                                                                  //执行应用程序
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
        SendBuf[SendLen++] = 0x02;                                                                     //校验错误
    }
    if((0x00 != tmpaddr) && (SlaveAddr == tmpaddr) || (0xFF == tmpaddr))
    {
        if(0x0004 == msgid)
        {
            TimingDelay_SetTime(50);//延迟应答
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
	FlashReadNHalfWord(FLASH_BASE_ADDR, (uint16_t *)&FlashPara, sizeof(FlashPara)>>3);						// 将Flash当前参数读出
	EN_INT;
	UpgradeInfo=FlashPara.Common.UpgradeFlag;                                                              //读取升级信息
	SlaveAddr=FlashPara.Common.Addr;
    ProgErase=FlashPara.Common.ProgEraseFlag;
    if((0x0C == UpgradeInfo) || (0xFF == UpgradeInfo) || (0x0C == ProgErase))   //如果有升级请求则无限等待
    {
        UpgradeWaitTime = 0;
    }
    else                                                                        //否则等待2秒
    {
        UpgradeWaitTime = 2;
    }

    TimingDelay_SetTime(1000);

    while(1)
    {
        if(TimingDelay_TimeOut() && UpgradeWaitTime && (10 > UpgradeWaitTime))  //判断是否无限等待
        {
            if(1 == UpgradeWaitTime)
            {
                break;
            }
            UpgradeWaitTime--;
            TimingDelay_SetTime(1000);
        }
        if(10 == UpgradeWaitTime)                                                   //升级完成跳出
        {
            break;
        }
        if(0x0C == OneFrameOk)                                                      //如果有数据则处理                                                      
        {
            OneFrameOk = 0;
            HandleFrameInformation();                                                 //处理接收的帧数据
			
        }
    }
    if (((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
    {
        //uprintf("APP error!\r\n");
        
		DIS_INT;
		FlashReadNHalfWord(FLASH_BASE_ADDR, (uint16_t *)&FlashPara, sizeof(FlashPara)>>1);		// 将Flash当前参数读出
        FlashPara.Common.UpgradeFlag=0x00;
		FlashPageErase(FLASH_BASE_ADDR);										// Flash写数据之前 必须先擦除
		FlashWriteDoubleWord(FLASH_BASE_ADDR, (uint64_t *)&FlashPara, sizeof(FlashPara)>>3);       // 将当前数据写入Flash当前参数区	
		EN_INT;																	// 总中断失能到使能必须从擦除开始到写入成功
        
		DIS_INT;
		FlashReadNHalfWord(FLASH_BASE_ADDR, (uint16_t *)&FlashPara, sizeof(FlashPara)>>1);		// 将Flash当前参数读出
        FlashPara.Common.ProgEraseFlag=0xC0;
		FlashPageErase(FLASH_BASE_ADDR);										// Flash写数据之前 必须先擦除
		FlashWriteDoubleWord(FLASH_BASE_ADDR, (uint64_t *)&FlashPara, sizeof(FlashPara)>>3);       // 将当前数据写入Flash当前参数区	
		EN_INT;																	// 总中断失能到使能必须从擦除开始到写入成功
        JumpToUserApplication();                                                    //执行应用程序
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
