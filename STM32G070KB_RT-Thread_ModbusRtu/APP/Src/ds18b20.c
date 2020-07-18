#include "ds18b20.h"
#include "stm32g0xx_hal.h"

/**
 * ����         : delay_us(uint32_t tim)	
 * ��������     : 
 * ����         : 
 * ����         : ΢����ʱ
 * �������     : tim=΢��
 * �������     : ��
 * ���ؽ��     : ��
 * ע���˵��   : ��
 * �޸�����     : �� 
 */
void delay_us(uint32_t tim)
{
	tim = tim*6;
	while(tim--);	
}


/**
 * ����         : delay_750us(void)
 * ��������     : 
 * ����         : 
 * ����         : 750΢����ʱ
 * �������     : ��
 * �������     : ��
 * ���ؽ��     : ��
 * ע���˵��   : ��
 * �޸�����     : �� 
 */
void delay_750us(void)
{
	uint32_t tim=9502;
	while(tim--);	
}

/**
 * ����         : delay_15us(void)
 * ��������     : 
 * ����         : 
 * ����         : 15΢����ʱ
 * �������     : ��
 * �������     : ��
 * ���ؽ��     : ��
 * ע���˵��   : ��
 * �޸�����     : �� 
 */
void delay_15us(void)
{
	uint32_t tim=182;
	while(tim--);	
}

/**
 * ����         : delay_12us(void)
 * ��������     : 
 * ����         : 
 * ����         : 12΢����ʱ
 * �������     : ��
 * �������     : ��
 * ���ؽ��     : ��
 * ע���˵��   : ��
 * �޸�����     : �� 
 */
void delay_12us(void)
{
	uint32_t tim=145;
	while(tim--);	
}


/**
 * ����         : delay_50us(void)
 * ��������     : 
 * ����         : 
 * ����         : 50΢����ʱ
 * �������     : ��
 * �������     : ��
 * ���ؽ��     : ��
 * ע���˵��   : ��
 * �޸�����     : �� 
 */
void delay_50us(void)
{
	uint32_t tim=635;
	while(tim--);	
}

/**
 * ����         : delay_60us(void)
 * ��������     : 
 * ����         : 
 * ����         : 60΢����ʱ
 * �������     : ��
 * �������     : ��
 * ���ؽ��     : ��
 * ע���˵��   : ��
 * �޸�����     : �� 
 */
void delay_60us(void)
{
	uint32_t tim=765;
	while(tim--);	
}


/**
 * ����         : DS18B20_IO_OUT()	
 * ��������     : 
 * ����         : 
 * ����         : ����ds18b20�ܽ�Ϊ���ģʽ
 * �������     : ��
 * �������     : ��
 * ���ؽ��     : ��
 * ע���˵��   : IO��������
 * �޸�����     : �� 
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
 * ����         : S18B20_IO_IN()	
 * ��������     : 
 * ����         : 
 * ����         : ����ds18b20�ܽ�Ϊ����ģʽ
 * �������     : ��
 * �������     : ��
 * ���ؽ��     : ��
 * ע���˵��   : IO��������
 * �޸�����     : �� 
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


/**
 * ����         : DS18B20_DQ_OUT(uint8_t out)	
 * ��������     : 
 * ����         : 
 * ����         : ����ds18b20�ܽŵ����״̬
 * �������     : 1	����ߵ�ƽ		���� ����͵�ƽ
 * �������     : ��
 * ���ؽ��     : ��
 * ע���˵��   : ��
 * �޸�����     : �� 
 */
void DS18B20_DQ_OUT(uint8_t out)
{
	if(1 == out)
		HAL_GPIO_WritePin(DS18B20_GPIOX, DS18B20_GPIO_PINX,GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(DS18B20_GPIOX, DS18B20_GPIO_PINX,GPIO_PIN_RESET);
}



/**
 * ����         : DS18B20_Rst(void)	
 * ��������     : 
 * ����         : 
 * ����         : ��λDS18B20
 * �������     : ��
 * �������     : ��
 * ���ؽ��     : ��
 * ע���˵��   : ��
 * �޸�����     : �� 
 */
void DS18B20_Rst(void)	   
{                 
	DS18B20_IO_OUT();   	//����Ϊ���
	DS18B20_DQ_OUT(0);  	//����DQ
	delay_750us();
	DS18B20_DQ_OUT(1);  	//DQ=1 
	delay_15us();
}



/**
 * ����         : DS18B20_Check(void)   
 * ��������     : 
 * ����         : 
 * ����         : �ȴ�DS18B20�Ļ�Ӧ
 * �������     : ��
 * �������     : ��
 * ���ؽ��     : 1:δ��⵽DS18B20�Ĵ���  0:����
 * ע���˵��   : ��
 * �޸�����     : �� 
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



/**
 * ����         : DS18B20_Read_Bit(void)     
 * ��������     : 
 * ����         : 
 * ����         : ��DS18B20��ȡһ��λ
 * �������     : ��
 * �������     : ��
 * ���ؽ��     : 1/0
 * ע���˵��   : ��
 * �޸�����     : �� 
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



/**
 * ����         : DS18B20_Read_Byte(void)     
 * ��������     : 
 * ����         : 
 * ����         : ��DS18B20��ȡһ���ֽ�
 * �������     : ��
 * �������     : ��
 * ���ؽ��     : ����������
 * ע���˵��   : ��
 * �޸�����     : �� 
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


/**
 * ����         : DS18B20_Write_Byte(uint8_t dat)     
 * ��������     : 
 * ����         : 
 * ����         : дһ���ֽڵ�DS18B20
 * �������     : dat��Ҫд����ֽ�
 * �������     : ��
 * ���ؽ��     : ��
 * ע���˵��   : ��
 * �޸�����     : �� 
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
 * ����         : DS18B20_Start(void)
 * ��������     : 
 * ����         : 
 * ����         : ��ʼ�¶�ת��
 * �������     : ��
 * �������     : ��
 * ���ؽ��     : ��
 * ע���˵��   : ��
 * �޸�����     : �� 
 */
void DS18B20_Start(void)
{   						               
    DS18B20_Rst();	   
    DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0x44);// convert
}


/**
 * ����         : DS18B20_Init(void)
 * ��������     : 
 * ����         : 
 * ����         : ��ʼ��DS18B20��IO�� DQ ͬʱ���DS�Ĵ���
 * �������     : ��
 * �������     : ��
 * ���ؽ��     : 1:������ 0:����
 * ע���˵��   : ��
 * �޸�����     : �� 
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


/**
 * ����         : DS18B20_Get_Temp(void)
 * ��������     : 
 * ����         : 
 * ����         : ��ȡ�¶�ֵ
 * �������     : ��
 * �������     : ��
 * ���ؽ��     : �¶�ֵ ��-550~1250��
 * ע���˵��   : ��
 * �޸�����     : �� 
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
