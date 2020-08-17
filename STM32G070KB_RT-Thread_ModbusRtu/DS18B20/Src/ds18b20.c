/**@file        ds18b20.c
* @brief        ds18b20模块的应用
* @details      ds18b20温度数据获取与处理
* @author       马灿林
* @date         2020-07-17
* @version      V1.0.0
* @copyright    2020-2030,深圳市信为科技发展有限公司
**********************************************************************************
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/07/17  <td>1.0.0    <td>马灿林    <td>创建初始版本
* </table>
*
**********************************************************************************
*/

#include "ds18b20.h"

/**@brief       微秒延时函数
* @param[in]    tim : 延时的微秒时间
* @return       无
*/
void delay_us(uint32_t tim)
{
	tim = tim*6;
	while(tim--);	
}


/**@brief       750微秒延时
* @param[in]    无
* @return       无
*/
void delay_750us(void)
{
	uint32_t tim=9502;
	while(tim--);	
}

/**@brief       15微秒延时
* @param[in]    无
* @return       无
*/
void delay_15us(void)
{
	uint32_t tim=182;
	while(tim--);	
}

/**@brief       12微秒延时
* @param[in]    无
* @return       无
*/
void delay_12us(void)
{
	uint32_t tim=145;
	while(tim--);	
}


/**@brief       50微秒延时
* @param[in]    无
* @return       无
*/
void delay_50us(void)
{
	uint32_t tim=635;
	while(tim--);	
}


/**@brief       60微秒延时
* @param[in]    无
* @return       无
*/
void delay_60us(void)
{
	uint32_t tim=765;
	while(tim--);	
}


/**
* @brief 配置管脚为推挽输出模式
*/
void DS18B20_IO_OUT()
{
	GPIO_InitTypeDef GPIO_Initure;
    RCC_GPIOX_CLK;										///开启GPIOA时钟
	
    GPIO_Initure.Pin=DS18B20_GPIO_PINX;           		///A15
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  			///推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;         				///上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;     		///高速
    HAL_GPIO_Init(DS18B20_GPIOX,&GPIO_Initure);     	///初始化
}


/**
* @brief 配置管脚为浮空输入模式
*/
void DS18B20_IO_IN()
{

	GPIO_InitTypeDef GPIO_Initure;
    RCC_GPIOX_CLK;										///开启GPIOA时钟
	
    GPIO_Initure.Pin=DS18B20_GPIO_PINX;     			///PA15
    GPIO_Initure.Mode=GPIO_MODE_INPUT;  				///浮空输入
    GPIO_Initure.Pull=GPIO_PULLUP;          			///上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;    		///高速
    HAL_GPIO_Init(DS18B20_GPIOX,&GPIO_Initure);     	///初始化
}


/**@brief       设置ds18b20管脚的输出状态
* @param[in]    out : 输出状态
* - None
* @note         out: 1	(输出高电平)	其他 (输出低电平)
*/
void DS18B20_DQ_OUT(uint8_t out)
{
	if(1 == out)
		HAL_GPIO_WritePin(DS18B20_GPIOX, DS18B20_GPIO_PINX,GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(DS18B20_GPIOX, DS18B20_GPIO_PINX,GPIO_PIN_RESET);
}




/**
* @brief 复位DS18B20
*/
void DS18B20_Rst(void)	   
{                 
	DS18B20_IO_OUT();   	//设置为输出
	DS18B20_DQ_OUT(0);  	//拉低DQ
	delay_750us();
	DS18B20_DQ_OUT(1);  	//DQ=1 
	delay_15us();
}

/**@brief       等待DS18B20的回应
* @return       函数执行结果
* - 返回结果    : 1 (未检测到DS18B20的存在)  0 (存在)
*/
uint8_t DS18B20_Check(void) 	   
{   
	uint8_t retry=0;
	DS18B20_IO_IN();    //设置为输入
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

/**@brief       从DS18B20读取一个位
* @return       函数执行结果,读取到的Bit 1 / 0 
*/
uint8_t DS18B20_Read_Bit(void) 
{
	uint8_t data;
	DS18B20_IO_OUT();   //设置为输出
	DS18B20_DQ_OUT(0); 
	delay_us(2);
	DS18B20_DQ_OUT(1); 
	DS18B20_IO_IN();    //设置为输入
	delay_12us();
	if(DS18B20_DQ_IN)data=1;
	else data=0;	 
	delay_50us();           
	return data;
}



/**@brief       从DS18B20读取一个字节
* @return       读到的数据Byte 
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


/**@brief       写一个字节到DS18B20
* @param[in]    dat : 要写入的字节
*/
void DS18B20_Write_Byte(uint8_t dat)     
 {             
    uint8_t j;
    uint8_t testb;
    DS18B20_IO_OUT();     //设置为输出
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if(testb)       // 写1
        {
            DS18B20_DQ_OUT(0);
            delay_us(2);                            
            DS18B20_DQ_OUT(1);
            delay_60us();            
        }
        else            //写0
        {
            DS18B20_DQ_OUT(0);
            delay_60us();             
            DS18B20_DQ_OUT(1);
            delay_us(2);                         
        }
    }
}
 

/**
* @brief 开始温度转换
*/
void DS18B20_Start(void)
{   						               
    DS18B20_Rst();	   
    DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0x44);// convert
}


/**@brief       初始化DS18B20的IO口DQ  同时检测DS的存在
* @return       DS18B20是否存在	1 (不存在) 0 (存在);
*/
uint8_t DS18B20_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
    RCC_GPIOX_CLK;									///开启GPIOA时钟
	
    GPIO_Initure.Pin=DS18B20_GPIO_PINX;     		///PA15
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  		///推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          		///上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;     	///高速
    HAL_GPIO_Init(DS18B20_GPIOX,&GPIO_Initure);     ///初始化
 
	DS18B20_Rst();
	return DS18B20_Check();
}


/**@brief       获取温度值
* @return       函数执行结果
* - 温度值 (-550~1250)	返回值是温度的十倍 (24.5℃时返回245)	
*/
short DS18B20_Get_Temp(void)
{
    uint8_t temp;
    uint8_t TL,TH;
    short tem;
    DS18B20_Start ();           ///开始转换
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
        temp=0;					///温度为负  
    }else temp=1;				///温度为正	  	  
    tem=TH; 					///获得高八位
    tem<<=8;    
    tem+=TL;					///获得底八位
    tem=(double)tem*0.625;		///转换     
	if(temp)return tem; 		///返回温度值
	else return -tem;    
}
