#include "drv_usart.h"
#include "modbus_asc.h"
#include "modbus_ascii.h"
//#include "type.h"
#include "eeprom.h"
#include "para.h"
//#include "DS18B20.h"
#include "sensor_dallas_ds18b20.h"

/************************************************************************************************************************************************************************
** ��Ȩ��   2016-2026, ��������Ϊ�Ƽ���չ���޹�˾
** �ļ���:  modbus_asc.c
** ����:    ׯ��Ⱥ
** �汾:    V1.0.0
** ����:    2016-09-05
** ����:    modbus ascii ������
** ����:         
*************************************************************************************************************************************************************************
** �޸���:          No
** �汾:  		
** �޸�����:    No 
** ����:            No
*************************************************************************************************************************************************************************/

uint8_t SendLenT;
uint8_t SendBufT[200];
uint32_t AlarmLastSta;
extern uint8_t UART1_RXBuff[USART1_MAX_DATALEN];
extern UserTypeDef UserPara;
extern uint8_t u8SendNum; 
extern FlagStatus UartRecvFrameOK;
extern uint32_t TemValue;
extern FlagStatus ReadDataFlag;            
extern FlagStatus StaChangeFlag;
//**************************************************************************************************
// ����         : MBASC_SendMsg()
// ��������     : 2016-09-05
// ����         : ׯ��Ⱥ
// ����         : ��ASCII��ʽ������Ϣ
// �������     : ֡������(u8 *u8Msg),֡��(u8 u8MsgLen)
// �������     : ��
// ���ؽ��     : ��
// ע���˵��   : 
// �޸�����     :
//**************************************************************************************************
extern uint8_t u8SendNum;

void MBASC_SendMsg(uint8_t *u8Msg, uint8_t u8MsgLen)
{
  
  rt_thread_mdelay(50);
    if(MB_ADDRESS_BROADCAST != u8Msg[0])
    {
        MODBUS_ASCII_SendData(u8Msg, u8MsgLen);
    }
}

//**************************************************************************************************
// ����         : MBASC_SendMsg_NoLimit()
// ��������     : 2016-09-05
// ����         : ׯ��Ⱥ
// ����         : �������Ե���ASCII��ʽ������Ϣ
// �������     : ֡������(u8 *u8Msg),֡��(u8 u8MsgLen)
// �������     : ��
// ���ؽ��     : ��
// ע���˵��   : 
// �޸�����     :
//**************************************************************************************************
void MBASC_SendMsg_NoLimit(uint8_t *u8Msg, uint8_t u8MsgLen)
{
   
    rt_thread_mdelay(50);
  MODBUS_ASCII_SendData(u8Msg, u8MsgLen);
}

//**************************************************************************************************
// ����         : MBASC_SendErr()
// ��������     : 2016-09-05
// ����         : ׯ��Ⱥ
// ����         : ���ʹ��������Ӧ֡
// �������     : ������(u8 ErrCode)
// �������     : ��
// ���ؽ��     : ��
// ע���˵��   : 
// �޸�����     :
//**************************************************************************************************
void MBASC_SendErr(uint8_t ErrCode)
{
    SendLenT = 1;
    SendBufT[SendLenT++] |= 0x80;
    SendBufT[SendLenT++] = ErrCode;
	//21 4 8
	rt_kprintf("MBASC_SendErr2 %x %x %x\n",SendBufT[0],SendBufT[1],SendBufT[2]);
    MBASC_SendMsg_NoLimit(SendBufT, SendLenT);
}

//**************************************************************************************************
// ����         : MBASC_Fun03()
// ��������     : 2016-09-05
// ����         : ׯ��Ⱥ
// ����         : 03�����룬�������Ĵ�������
// �������     : ��
// �������     : ��
// ���ؽ��     : ��
// ע���˵��   : 
// �޸�����     :  1.���ӻ���ַ�޸�Ϊ0x45����Ӧ�ļĴ�����ַ�޸�Ϊ0x45xx   2016.09.08
//                 2.�޸Ĵӻ���ַ�󣬶�Ӧ�ļĴ��ַ���Ա仯���磺�޸�Ϊ0x46,����Զ�ȡ0x4630��ַ����
//**************************************************************************************************
void MBASC_Fun03(void)
{
    uint8_t i;
    uint16_t Data_Buf;
    uint16_t Register_Num = 0;
    uint8_t ReadAdrH, ReadAdrL;
    
    ReadAdrH = UART1_RXBuff[2];
    ReadAdrL = UART1_RXBuff[3];

    Register_Num = ((uint16_t)UART1_RXBuff[4] << 8) + UART1_RXBuff[5];
     
    SendLenT = 0;
    SendBufT[SendLenT++] = UART1_RXBuff[0] ? UserPara.SlaveAddr : 0x00;
    SendBufT[SendLenT++] = 0x03;	                        //������
    SendBufT[SendLenT++] = Register_Num * 2;		                        //���ݳ���
                                                                                //�����ȡ��Χ���
    if ((!(((ReadAdrL >= MBASC_HOLDING_REG_REGION_BGEIN) && (ReadAdrL <= MBASC_HOLDING_REG_REGION_END)
        && (ReadAdrL + Register_Num <= (MBASC_HOLDING_REG_REGION_END + 1)))&& (0 != Register_Num)))
        || ((ReadAdrH != UserPara.SlaveAddr) && (ReadAdrH != MB_ADDRESS_BROADCAST)))
    {
        MBASC_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
        return;
    }

    for (uint8_t k = 0; k < Register_Num; ReadAdrL++, k++)
    {
        switch (ReadAdrL)
        {
          case 0x30:
            Data_Buf = UserPara.SlaveAddr;				        //�豸��ַ
            break;

          case 0x31:
            Data_Buf = 3;				        
            break;

          case 0x32:
            Data_Buf = 3;			                        
            break;

          case 0x33:
            Data_Buf = 0;						        
            break;

          case 0x34:
            Data_Buf = 1;	                        
            break;

          case 0x35:                                                          
            Data_Buf = 2;			                                             
            break;

          case 0x36:
            Data_Buf =0;		               
            break;

          case 0x37:
            Data_Buf = 0;                       
            break;

          case 0x38:
            Data_Buf = 0;                        
            break;

          case 0x39:
            Data_Buf = 0;	                                                                 
            break;

          case 0x3A:
            Data_Buf = 0;	                                                             
            break;  
            
          case 0x3B:
            Data_Buf = 0;	                                                             
            break;             
            
          case 0x3C:
            Data_Buf = 0;	                                                             
            break;
            
          case 0x3D:
            Data_Buf = 0;				  	                                                             
            break;             
            
          case 0x3E:
            Data_Buf = 0;	                                                             
            break;
            
          case 0x3F:
            Data_Buf = 0;                                                  
            break;
            
          case 0x40:
            Data_Buf = UserPara.Up_Thr;	//�¶ȱ�������ֵ	        
            break;

          case 0x41:                                                        
            Data_Buf = UserPara.Do_Thr; //�¶ȱ�������ֵ
            break;
            
          case 0x42:
            Data_Buf = UserPara.Du_Thr; //����ʱ����ֵ                                
            break; 
            
          case 0x43:
            Data_Buf = 0;
            break;  
                
          case 0x44:
            Data_Buf = 0;
            break;
                
          case 0x45:
            Data_Buf = 0;
            break;
                
          case 0x46:
            Data_Buf = 0;
            break;
                
          case 0x47:
            Data_Buf = 0;
            break;
                
          case 0x48:
            Data_Buf = 0;
            break;
                
          case 0x49:
            Data_Buf = 0;
            break;
                
          case 0x4A:
            Data_Buf = 0;
            break;
                
          case 0x4B:
            Data_Buf = 0;
            break;
                
          case 0x4C:
            Data_Buf = 0;
            break;
                
          case 0x4D:
            Data_Buf = 0;
            break;
                
          case 0x4E:
            Data_Buf = 0;
            break;
                
          case 0x4F:
            Data_Buf = 0;
            break; 
            
          default:
            Data_Buf = 0;
            break;
        }
        for (i = 2; i > 0; i--)
        {
            SendBufT[SendLenT++] = (uint8_t)(Data_Buf >> ((i - 1) * 8));
        }
    }
    MBASC_SendMsg_NoLimit(SendBufT, SendLenT);                                      
}



//**************************************************************************************************
// ����         : MBASC_Fun04()
// ��������     : 2016-09-05
// ����         : ׯ��Ⱥ
// ����         : 04�����룬��˫���Ĵ�������
// �������     : ��
// �������     : ��
// ���ؽ��     : ��
// ע���˵��   : 
// �޸�����     :  ����Э���޸����ݷ��ͷ�ʽ,���͸������ݸ�Ϊ��������     2016.09.12
//**************************************************************************************************
void MBASC_Fun04(void)	
{
    uint8_t i;
    uint32_t Data_Buf;
    uint16_t Register_Num ;
    uint8_t ReadAdrH, ReadAdrL;
    
    ReadAdrH = UART1_RXBuff[2];
    ReadAdrL = UART1_RXBuff[3];
	//3A 32 31 30 34 30 34 30 30 30 30 30 42 42 44 30 46 0D 0A 
	
    Register_Num =8; //((uint16_t)UART1_RXBuff[4] <<8) + UART1_RXBuff[5];
    rt_kprintf("a����%02x,%02x,%02x,%02x,%02x,%d\n",ReadAdrH,ReadAdrL,UART1_RXBuff[4],UART1_RXBuff[5],UART1_RXBuff[6],Register_Num);
    SendLenT = 0;
    SendBufT[SendLenT++] = UART1_RXBuff[0] ? UserPara.SlaveAddr : 0x00;
	  rt_kprintf("enc_SendErr %x %x %x\n",UART1_RXBuff[0],UserPara.SlaveAddr,SendBufT[0]);
    SendBufT[SendLenT++] = 0x04;
    SendBufT[SendLenT++] = 4*2;//Register_Num * 2;		                        //���ݳ���
	  rt_kprintf("enc_SendErr %x %x\n",SendBufT[1],SendBufT[2]);

//    if((!((((ReadAdrL <= MBASC_INPUT_REG_REGION_END) && ((ReadAdrL + Register_Num) <= (MBASC_INPUT_REG_REGION_END + 2))))
//    && ((0 != Register_Num) && (0 == (Register_Num & 0x01)) && (0 == (ReadAdrL & 0x01)))))
//    || (ReadAdrH != UserPara.SlaveAddr))   
//    {    
//       
//        MBASC_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
//        return;
//    }   
    rt_kprintf("SendLenT %d\n",SendLenT);
     //a����04,00,00,0b,c8,11
    for (uint8_t k = 0; k < Register_Num; ReadAdrL += 2, k += 2)
    {
        switch (ReadAdrL)
        {
          case 0x00:
            Data_Buf = (uint32_t)((UserPara.Temp + 27310000) / 10000);
            if((StaChangeFlag == SET) && (UserPara.AlarmSta != 0))   //��������������Ҫ����
            {
                Data_Buf |= 0x80000000;     //��Ҫ����bit31��λ������1600ms
                ReadDataFlag = SET;
            }
            /*if(StaChangeFlag == RESET)
            {
                Data_Buf &= 0x7FFFFFFF;
            }*/
            break;

          case 0x02:                                                                               
            Data_Buf = UserPara.Duration;
			
		  rt_kprintf("UserPara.Duration %d\n",Data_Buf);
            break;

          case 0x04: 
            Data_Buf = UserPara.AlarmSta;
			
		  rt_kprintf("UserPara.AlarmSta %d\n",UserPara.AlarmSta);
            //AlarmLastSta = UserPara.AlarmSta;
            break;
            
          default:
            Data_Buf = 0;
            break;
        }
        for(i = 4; i > 0; i--)
        {
            SendBufT[SendLenT++] = (uint8_t)(Data_Buf >> ((i - 1) * 8));
        }
    }
    MBASC_SendMsg(SendBufT, SendLenT);
}




//**************************************************************************************************
// ����         : MBASC_Fun10()
// ��������     : 2016-09-05
// ����         : ׯ��Ⱥ
// ����         : 10�����룬д����Ĵ��������������޸Ĳ���
// �������     : ��
// �������     : ��
// ���ؽ��     : ��
// ע���˵��   : 
// �޸�����     : 1.�Ƚ����Զ��ϴ�ʱ����룬�ٰѱ���ת����ʱ�䣬�����ָ���޸ı��벻�������Զ��ϴ�ʱ
//                �䲻����Ҫ��ʱ������      2016.09.10
//                2.���Ӵ��ڳ�ʼ�����޸Ĳ����ʺ���Բ�������Ƭ����
//**************************************************************************************************
void MBASC_Fun10()
{
    uint32_t index = 0;
    uint16_t Register_Num = 0;
    uint8_t ReadAdrH, ReadAdrL;
    
    ReadAdrH = UART1_RXBuff[2];
    ReadAdrL = UART1_RXBuff[3];
        
    Register_Num = ((uint16_t)UART1_RXBuff[4] << 8) + UART1_RXBuff[5]; 
    
    SendLenT = 0;
    SendBufT[SendLenT++] = UART1_RXBuff[0] ? UserPara.SlaveAddr : 0x00;
    SendBufT[SendLenT++] = 0x10;
    SendBufT[SendLenT++] = Register_Num * 2;
                                                                                //�����ȡ��Χ���
    if ((!((((ReadAdrL >= MBASC_MUL_REG_REGION_BGEIN) && (ReadAdrL <= MBASC_MUL_REG_REGION_END)
            && (ReadAdrL + Register_Num <= (MBASC_MUL_REG_REGION_END + 1)))
            || (ReadAdrL == 0x70))
            && ((0 != Register_Num)) && ((Register_Num * 2) == UART1_RXBuff[6])))
            || ((ReadAdrH != UserPara.SlaveAddr) && (ReadAdrH != MB_ADDRESS_BROADCAST)))
    {
        MBASC_SendErr(MB_EX_ILLEGAL_DATA_ADDRESS);
        return;
    }

    for (uint8_t k = 0; k < Register_Num; ReadAdrL++, k++)
    {
        switch (ReadAdrL)
        {
            case 0x30:						                //�豸��ַ
                UserPara.SlaveAddr = ((uint16_t)UART1_RXBuff[7+index] << 8) + UART1_RXBuff[8+index];
                if(UserPara.SlaveAddr >= 33 && UserPara.SlaveAddr <= 37)
                {
                    Eeprom_WriteNBytes(SLAVE_ADDR, &UserPara.SlaveAddr , 1);
                }
                break;

            case 0x31:						                  
                break;

            case 0x32:						             
                break;

            case 0x33:                                                             
                break;

            case 0x34:                                                              
                break;

            case 0x35:						                
                break;

            case 0x36:                                                              
                break;

            case 0x37:                                                           
                break;

            case 0x38:
                break;

            case 0x39:						                
                break;

            case 0x3A:
                break; 

            case 0x3B:						                
                break;

            case 0x3C:						                
                break;

            case 0x3D:						             
                break;

            case 0x3E:						                
                break; 

            case 0x3F:						               
                break;       

            case 0x40:
                UserPara.Up_Thr = ((uint16_t)UART1_RXBuff[7+index] << 8) + UART1_RXBuff[8+index];
                Eeprom_WriteNBytes(TEM_UP_THR, (uint8_t*)&UserPara.Up_Thr, 2);
                break; 

            case 0x41:
                UserPara.Do_Thr = ((uint16_t)UART1_RXBuff[7+index] << 8) + UART1_RXBuff[8+index];
                Eeprom_WriteNBytes(TEM_DO_THR, (uint8_t*)&UserPara.Do_Thr, 2);
                break;

            case 0x42:
                UserPara.Du_Thr = ((uint16_t)UART1_RXBuff[7+index] << 8) + UART1_RXBuff[8+index];
                Eeprom_WriteNBytes(TEM_DU_THR, (uint8_t*)&UserPara.Du_Thr, 2);
                break;                    

            case 0x43:						               
                break;  
                
            case 0x44:						               
                break;
                
            case 0x45:						               
                break;
                
            case 0x46:						               
                break;
                
            case 0x47:						               
                break;
                
            case 0x48:						               
                break;
                
            case 0x49:						               
                break;
                
            case 0x4A:						               
                break;
                
            case 0x4B:						               
                break;
                
            case 0x4C:						               
                break;
                
            case 0x4D:						               
                break;
                
            case 0x4E:						               
                break;
                
            case 0x4F:						               
                break;                

            case 0x70:						               
                break; 
                
            default:
                break;
        }
        index += 2;
    }
    
    MBASC_SendMsg(UART1_RXBuff, 6);
}





                
//**************************************************************************************************
// ����         : MBASC_Fun41()
// ��������     : 2016-09-19
// ����         : ÷����
// ����         : ����ϵͳ����boot��ʼִ�У�
// �������     : ��
// �������     : ��
// ���ؽ��     : ��
// ע���˵��   : 
// �޸�����     :
//**************************************************************************************************
void MBASC_Fun41(void)
{
    uint16_t ReadAdr = 0;
    uint16_t DataLength = 0;
    uint8_t ReadData;
      
    ReadAdr = ((uint16_t)(UART1_RXBuff[2]<< 8)) + UART1_RXBuff[3];
    DataLength = ((uint16_t)(UART1_RXBuff[4]<< 8)) + UART1_RXBuff[5];
        
    SendLenT = 0;
    SendBufT[SendLenT++] = UART1_RXBuff[0] ? UserPara.SlaveAddr : MB_ADDRESS_BROADCAST;
    SendBufT[SendLenT++] = 0x41;
    for(uint8_t i = 2; i < 6; i++)
    {
        SendBufT[SendLenT++] = UART1_RXBuff[i];
    }
                                                    //��ֹ�㲥��ַʹ�������boot
    if((0x0001 != ReadAdr) || (0 != DataLength) || (UART1_RXBuff[0] == MB_ADDRESS_BROADCAST))
    {
        return;
    }
    else 
    {
        Eeprom_WriteNBytes(1023,"\x0C", 1);
        Eeprom_ReadNBytes(1023, &ReadData, 1);
        if(ReadData == 0x0C)
        {
            SendBufT[SendLenT++] = 0x00;
            MBASC_SendMsg(SendBufT, SendLenT);
            while(0 != u8SendNum);      //�ȴ����ݷ�����ϣ��˾�Ҫ���ϣ�����ִ�д˹�����������Ӧ
            NVIC_SystemReset();
        }
        else
        {
            SendBufT[SendLenT++] = 0x01;
            MBASC_SendMsg(SendBufT, SendLenT); 
        }
    } 
}     
     

//**************************************************************************************************
// ����         : MBASC_Function()
// ��������     : 2016-09-05
// ����         : ׯ��Ⱥ
// ����         : modbus asciiͨ�Ŵ���
// �������     : ��
// �������     : ��
// ���ؽ��     : ��
// ע���˵��   : 
// �޸�����     :  
//                                         
//**************************************************************************************************
void MBASC_Function(void)
{
    uint8_t RecvLen = 0;
    if(UartRecvFrameOK == SET)
    {
        if(2 == MODBUS_ASCII_RecvData(UART1_RXBuff, &RecvLen))//У�����
        {rt_kprintf("У�����\n");
            return;
        }  
        if(RecvLen <= 0)
        {
        rt_kprintf("RecvLen <= 0\n");
            return;                                                                 //û�н��ܵ����ݣ�����Ӧ
        }

        else if((UserPara.SlaveAddr != UART1_RXBuff[0]) && (MB_ADDRESS_BROADCAST != UART1_RXBuff[0]))
        { rt_kprintf("���յĴӻ���ַ����Ӧ%x %x\n",UserPara.SlaveAddr,UART1_RXBuff[0]);
            return;                                                                 //���յĴӻ���ַ����Ӧ������Ӧ
        }

        else
        {
            switch (UART1_RXBuff[1])
            {
              case 0x03:
			  	rt_kprintf("MBASC_Fun03\n");
                MBASC_Fun03();	                                                //�������Ĵ��������ֽ����ݣ�
                break;

              case 0x04:
			  	
			  rt_kprintf("MBASC_Fun04\n");
                MBASC_Fun04();	                                                //��˫���Ĵ������������ݣ�
                break;
                
              case 0x10:
			  	
			  rt_kprintf("MBASC_Fun10\n");
                MBASC_Fun10();                                                  //д����Ĵ���
                break;               
     
              case 0x41:
			  	
			  rt_kprintf("MBASC_Fun41\n");
                MBASC_Fun41();
                break; 
                
              default:
			  	
			  rt_kprintf("default\n");
                SendLenT = 0;
                SendBufT[SendLenT++] = UART1_RXBuff[0];
                SendBufT[SendLenT++] = 0x80 | UART1_RXBuff[1];
                SendBufT[SendLenT++] = MB_EX_ILLEGAL_FUNCTION;
                MBASC_SendMsg(SendBufT, SendLenT);
                break;
             }
         }
    }
}
