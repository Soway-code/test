/**@file        ds18b20.c
* @brief        ds18b20ģ���Ӧ��
* @details      ds18b20�¶����ݻ�ȡ�봦��
* @author       �����
* @date         2020-08-01
* @version      V1.0.0
* @copyright    2020-2030,��������Ϊ�Ƽ���չ���޹�˾
**********************************************************************************
* @par �޸���־:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/08/01  <td>1.0.0    <td>�����    <td>������ʼ�汾
* </table>
*
**********************************************************************************
*/

#include "ds18b20.h"
#include "tim.h"
											   
#define	DS18B20_DQ_IN  HAL_GPIO_ReadPin(DS18B20_GPIOX,DS18B20_GPIO_PINX)  

#if 1 == USING_RT_THREAD_OS	

	rt_hw_interrupt_disable PROC
		EXPORT rt_hw_interrupt_disable
		CPSID I (1)
		BX LR
		ENDP
	
	rt_hw_interrupt_enable PROC
		EXPORT rt_hw_interrupt_enable
		CPSIE I (2)
		BX LR
		ENDP
	PRIMASK = 0; 					//PRIMASK ��ʼֵΪ 0,��ʾû�й��ж� 

#endif

/**
* @brief ���ùܽ�Ϊ�������ģʽ
*/
void DS18B20_IO_OUT()
{
	GPIO_InitTypeDef GPIO_Initure;
   // RCC_GPIOX_CLK;									///����GPIOAʱ��
	
    GPIO_Initure.Pin=DS18B20_GPIO_PINX;           		///A15
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  			///�������
    GPIO_Initure.Pull=GPIO_PULLUP;         				///����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;     		///����
    HAL_GPIO_Init(DS18B20_GPIOX,&GPIO_Initure);     	///��ʼ��
}

/**
* @brief ���ùܽ�Ϊ��������ģʽ
*/
void DS18B20_IO_IN()
{
	GPIO_InitTypeDef GPIO_Initure;
   // RCC_GPIOX_CLK;									//����GPIOAʱ��
	
    GPIO_Initure.Pin=DS18B20_GPIO_PINX;     			//PA15
    GPIO_Initure.Mode=GPIO_MODE_INPUT;  				//��������
    GPIO_Initure.Pull=GPIO_PULLUP;          			//����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;    		//����
    HAL_GPIO_Init(DS18B20_GPIOX,&GPIO_Initure);     	//��ʼ��
}


/**@brief       ����ds18b20�ܽŵ����״̬
* @param[in]    out : ���״̬
* - None
* @note         out: 1	(����ߵ�ƽ)	���� (����͵�ƽ)
*/
void DS18B20_DQ_OUT(uint8_t out)
{
	if(1 == out)
		HAL_GPIO_WritePin(DS18B20_GPIOX, DS18B20_GPIO_PINX,GPIO_PIN_SET);		//����ߵ�ƽ
	else
		HAL_GPIO_WritePin(DS18B20_GPIOX, DS18B20_GPIO_PINX,GPIO_PIN_RESET);		//����͵�ƽ
}

/**
* @brief ��λDS18B20
*/
void DS18B20_Rst(void)	   
{                 
	DS18B20_IO_OUT();   	//����Ϊ���
	DS18B20_DQ_OUT(0);  	//����DQ
	delay_tim_750us();		//��ʱ750us 480~960us
	DS18B20_DQ_OUT(1);  	//����DQ 
	delay_tim_15us();		//�ȴ��ӻ���Ӧʱ�� 15~60us
}

/**@brief       �ȴ�DS18B20�Ļ�Ӧ
* @return       ����ִ�н��
* - ���ؽ��    : 1 (δ��⵽DS18B20�Ĵ���)  0 (����)
*/
uint8_t DS18B20_Check(void) 	   
{   
	uint8_t retry=0;
	DS18B20_IO_IN();    				//����Ϊ����
    while (DS18B20_DQ_IN&&retry<160)	//�ȴ��͵�ƽ���嵽��ʱ�� 15~60us
	{
		retry++;
		delay_tim_1us();	
	};	 
	if(retry>=160)
		return 1;
	else 
		retry=0;
	
    while (!DS18B20_DQ_IN&&retry<200)	//�͵�ƽ�������ʱ�� 60~240us
	{
		retry++;
		delay_tim_1us();
	};
	if(retry>=200)
		return 1;	    
	return 0;
}

/**@brief       ��DS18B20��ȡһ��λ
* @return       ����ִ�н��,��ȡ����Bit 1 / 0 
*/
uint8_t DS18B20_Read_Bit(void) 
{
	uint8_t data;
	DS18B20_IO_OUT();   		//��DQ��������Ϊ�������ģʽ
	DS18B20_DQ_OUT(0); 			//����DQ 
	delay_tim_2us();			//���ж����ݲ���ǰ��������������������15us�ĵ͵�ƽ����
	DS18B20_DQ_OUT(1); 
								
	DS18B20_IO_IN();   		 	//����Ϊ����
	delay_tim_5us();
	delay_tim_5us();
	if(DS18B20_DQ_IN)
		data=1;
	else
		data=0;	
	
	delay_tim_15us();			//45us����������ʱ�䣨60~120us����ȥǰ��͵�ƽ����ʱ�� 
	delay_tim_15us();
	delay_tim_15us();
	return data;
}

/**@brief       ��DS18B20��ȡһ���ֽ�
* @return       ����������Byte 
*/
uint8_t DS18B20_Read_Byte(void)   
{        
	uint8_t i,j,dat;
	dat=0;
	for (i=1;i<=8;i++) 
	{
        j=DS18B20_Read_Bit();
        dat=(j<<7)|(dat>>1);
    }						    
	return dat;
}

/**@brief       дһ���ֽڵ�DS18B20
* @param[in]    dat : Ҫд����ֽ�
*/
void DS18B20_Write_Byte(uint8_t dat)     
{             
    uint8_t j;
    uint8_t testb;
    DS18B20_IO_OUT();			//����Ϊ���
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if(testb)       		// д1
        {
            DS18B20_DQ_OUT(0);	//����DQ 
			delay_tim_2us();	//д��1ǰ ��Ҫ���������� 1~15us        
            DS18B20_DQ_OUT(1);	//����DQ 
			delay_tim_60us();            
        }
        else           			 //д0
        {
            DS18B20_DQ_OUT(0);
            delay_tim_60us();            
            DS18B20_DQ_OUT(1);
			delay_tim_2us();	//�ָ�����һ������ �������������� 1us~����              
        }
		
	}
}
 
/**
* @brief ��ʼ�¶�ת��
*/
void DS18B20_Start(void)
{   						               
    DS18B20_Rst();	   			//��λds18b20������
    DS18B20_Check();	 		//���DS18B20�Ĵ���
    DS18B20_Write_Byte(0xcc);	// skip rom
    DS18B20_Write_Byte(0x44);	// convert
}

/**@brief       ��ʼ��DS18B20��IO��DQ  ͬʱ���DS�Ĵ���
* @return       DS18B20�Ƿ����	1 (������) 0 (����);	����0x10 ��ȡHCLKƵ��ʧ��;
*/
uint8_t DS18B20_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
    RCC_GPIOX_CLK;									//����GPIOAʱ��
	
    GPIO_Initure.Pin=DS18B20_GPIO_PINX;     		//PA15
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  		//�������
    GPIO_Initure.Pull=GPIO_PULLUP;          		//����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;     	//����
    HAL_GPIO_Init(DS18B20_GPIOX,&GPIO_Initure);     //��ʼ��
	if( 0 == Get_HCLKFreq() )						//��ȡHCLK��Ƶ��,Ƶ����ȷ����Ƶ��ֵ��ʧ�ܷ���ֵΪ 0 
		return 0x10;
	//printf("RCC=%d\n",Get_HCLKFreq());
	MX_TIMx_Init();									//��ʼ����ʱ��3
	DS18B20_Rst();									//��λds18b20������
	return DS18B20_Check();							//���DS18B20�Ĵ���
}

/**@brief       ��ȡ�¶�ֵ
* @return       ����ִ�н��
* - �¶�ֵ (-550~1250)	����ֵ���¶ȵ�ʮ�� (24.5��ʱ����245)	
*/
short DS18B20_Get_Temp(void)
{
    uint8_t temp;
    uint8_t TL,TH;
    short tem;
	
#if 1 == USING_RT_THREAD_OS	   
	rt_hw_interrupt_disable(); 	// �����ٽ������ж�,PRIMASK = 1	
#endif
	
	DS18B20_Start ();           //��ʼת��
    DS18B20_Rst();				//��λds18b20������
    DS18B20_Check();	 		//���DS18B20�Ĵ���
    DS18B20_Write_Byte(0xcc);   // skip rom
    DS18B20_Write_Byte(0xbe);   // convert	    
    TL=DS18B20_Read_Byte();     // LSB   
    TH=DS18B20_Read_Byte();     // MSB  

#if 1 == USING_RT_THREAD_OS	 	
	rt_hw_interrupt_enable(); 	//�˳��ٽ������ж�,PRIMASK = 0 
#endif
    
	if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;					//�¶�Ϊ��  
    }
	else 
	{	
		temp=1;					//�¶�Ϊ��
	}
	
    tem=TH; 					//��ø߰�λ
    tem<<=8;    
    tem+=TL;					//��õװ�λ
    tem=(double)tem*0.625;		//ת��     
	
	if(temp)
		return tem; 			//�����¶�ֵ
	else 
		return -tem;    
}
