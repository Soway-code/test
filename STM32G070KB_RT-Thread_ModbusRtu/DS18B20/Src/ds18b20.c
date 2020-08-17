/**@file        ds18b20.c
* @brief        ds18b20ģ���Ӧ��
* @details      ds18b20�¶����ݻ�ȡ�봦��
* @author       �����
* @date         2020-07-17
* @version      V1.0.0
* @copyright    2020-2030,��������Ϊ�Ƽ���չ���޹�˾
**********************************************************************************
* @par �޸���־:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/07/17  <td>1.0.0    <td>�����    <td>������ʼ�汾
* </table>
*
**********************************************************************************
*/

#include "ds18b20.h"

/**@brief       ΢����ʱ����
* @param[in]    tim : ��ʱ��΢��ʱ��
* @return       ��
*/
void delay_us(uint32_t tim)
{
	tim = tim*6;
	while(tim--);	
}


/**@brief       750΢����ʱ
* @param[in]    ��
* @return       ��
*/
void delay_750us(void)
{
	uint32_t tim=9502;
	while(tim--);	
}

/**@brief       15΢����ʱ
* @param[in]    ��
* @return       ��
*/
void delay_15us(void)
{
	uint32_t tim=182;
	while(tim--);	
}

/**@brief       12΢����ʱ
* @param[in]    ��
* @return       ��
*/
void delay_12us(void)
{
	uint32_t tim=145;
	while(tim--);	
}


/**@brief       50΢����ʱ
* @param[in]    ��
* @return       ��
*/
void delay_50us(void)
{
	uint32_t tim=635;
	while(tim--);	
}


/**@brief       60΢����ʱ
* @param[in]    ��
* @return       ��
*/
void delay_60us(void)
{
	uint32_t tim=765;
	while(tim--);	
}


/**
* @brief ���ùܽ�Ϊ�������ģʽ
*/
void DS18B20_IO_OUT()
{
	GPIO_InitTypeDef GPIO_Initure;
    RCC_GPIOX_CLK;										///����GPIOAʱ��
	
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
    RCC_GPIOX_CLK;										///����GPIOAʱ��
	
    GPIO_Initure.Pin=DS18B20_GPIO_PINX;     			///PA15
    GPIO_Initure.Mode=GPIO_MODE_INPUT;  				///��������
    GPIO_Initure.Pull=GPIO_PULLUP;          			///����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;    		///����
    HAL_GPIO_Init(DS18B20_GPIOX,&GPIO_Initure);     	///��ʼ��
}


/**@brief       ����ds18b20�ܽŵ����״̬
* @param[in]    out : ���״̬
* - None
* @note         out: 1	(����ߵ�ƽ)	���� (����͵�ƽ)
*/
void DS18B20_DQ_OUT(uint8_t out)
{
	if(1 == out)
		HAL_GPIO_WritePin(DS18B20_GPIOX, DS18B20_GPIO_PINX,GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(DS18B20_GPIOX, DS18B20_GPIO_PINX,GPIO_PIN_RESET);
}




/**
* @brief ��λDS18B20
*/
void DS18B20_Rst(void)	   
{                 
	DS18B20_IO_OUT();   	//����Ϊ���
	DS18B20_DQ_OUT(0);  	//����DQ
	delay_750us();
	DS18B20_DQ_OUT(1);  	//DQ=1 
	delay_15us();
}

/**@brief       �ȴ�DS18B20�Ļ�Ӧ
* @return       ����ִ�н��
* - ���ؽ��    : 1 (δ��⵽DS18B20�Ĵ���)  0 (����)
*/
uint8_t DS18B20_Check(void) 	   
{   
	uint8_t retry=0;
	DS18B20_IO_IN();    //����Ϊ����
    while (DS18B20_DQ_IN&&retry<200)
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=200)return 1;
	else retry=0;
    while (!DS18B20_DQ_IN&&retry<240)
	{
		retry++;
		delay_us(1);
	};
	if(retry>=240)return 1;	    
	return 0;
}

/**@brief       ��DS18B20��ȡһ��λ
* @return       ����ִ�н��,��ȡ����Bit 1 / 0 
*/
uint8_t DS18B20_Read_Bit(void) 
{
	uint8_t data;
	DS18B20_IO_OUT();   //����Ϊ���
	DS18B20_DQ_OUT(0); 
	delay_us(2);
	DS18B20_DQ_OUT(1); 
	DS18B20_IO_IN();    //����Ϊ����
	delay_12us();
	if(DS18B20_DQ_IN)data=1;
	else data=0;	 
	delay_50us();           
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
    DS18B20_IO_OUT();     //����Ϊ���
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if(testb)       // д1
        {
            DS18B20_DQ_OUT(0);
            delay_us(2);                            
            DS18B20_DQ_OUT(1);
            delay_60us();            
        }
        else            //д0
        {
            DS18B20_DQ_OUT(0);
            delay_60us();             
            DS18B20_DQ_OUT(1);
            delay_us(2);                         
        }
    }
}
 

/**
* @brief ��ʼ�¶�ת��
*/
void DS18B20_Start(void)
{   						               
    DS18B20_Rst();	   
    DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0x44);// convert
}


/**@brief       ��ʼ��DS18B20��IO��DQ  ͬʱ���DS�Ĵ���
* @return       DS18B20�Ƿ����	1 (������) 0 (����);
*/
uint8_t DS18B20_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
    RCC_GPIOX_CLK;									///����GPIOAʱ��
	
    GPIO_Initure.Pin=DS18B20_GPIO_PINX;     		///PA15
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  		///�������
    GPIO_Initure.Pull=GPIO_PULLUP;          		///����
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;     	///����
    HAL_GPIO_Init(DS18B20_GPIOX,&GPIO_Initure);     ///��ʼ��
 
	DS18B20_Rst();
	return DS18B20_Check();
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
    DS18B20_Start ();           ///��ʼת��
    DS18B20_Rst();
    DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);   /// skip rom
    DS18B20_Write_Byte(0xbe);   /// convert	    
    TL=DS18B20_Read_Byte();     /// LSB   
    TH=DS18B20_Read_Byte();     /// MSB   
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;					///�¶�Ϊ��  
    }else temp=1;				///�¶�Ϊ��	  	  
    tem=TH; 					///��ø߰�λ
    tem<<=8;    
    tem+=TL;					///��õװ�λ
    tem=(double)tem*0.625;		///ת��     
	if(temp)return tem; 		///�����¶�ֵ
	else return -tem;    
}
