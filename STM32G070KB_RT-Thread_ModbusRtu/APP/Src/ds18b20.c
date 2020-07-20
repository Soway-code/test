#include "ds18b20.h"
#include "stm32g0xx_hal.h"

/**
 * 名称         : delay_us(uint32_t tim)	
 * 创建日期     : 
 * 作者         : 
 * 功能         : 微秒延时
 * 输入参数     : tim=微秒
 * 输出参数     : 无
 * 返回结果     : 无
 * 注意和说明   : 无
 * 修改内容     : 无 
 */
void delay_us(uint32_t tim)
{
	tim = tim*6;
	while(tim--);	
}


/**
 * 名称         : delay_750us(void)
 * 创建日期     : 
 * 作者         : 
 * 功能         : 750微秒延时
 * 输入参数     : 无
 * 输出参数     : 无
 * 返回结果     : 无
 * 注意和说明   : 无
 * 修改内容     : 无 
 */
void delay_750us(void)
{
	uint32_t tim=9502;
	while(tim--);	
}

/**
 * 名称         : delay_15us(void)
 * 创建日期     : 
 * 作者         : 
 * 功能         : 15微秒延时
 * 输入参数     : 无
 * 输出参数     : 无
 * 返回结果     : 无
 * 注意和说明   : 无
 * 修改内容     : 无 
 */
void delay_15us(void)
{
	uint32_t tim=182;
	while(tim--);	
}

/**
 * 名称         : delay_12us(void)
 * 创建日期     : 
 * 作者         : 
 * 功能         : 12微秒延时
 * 输入参数     : 无
 * 输出参数     : 无
 * 返回结果     : 无
 * 注意和说明   : 无
 * 修改内容     : 无 
 */
void delay_12us(void)
{
	uint32_t tim=145;
	while(tim--);	
}


/**
 * 名称         : delay_50us(void)
 * 创建日期     : 
 * 作者         : 
 * 功能         : 50微秒延时
 * 输入参数     : 无
 * 输出参数     : 无
 * 返回结果     : 无
 * 注意和说明   : 无
 * 修改内容     : 无 
 */
void delay_50us(void)
{
	uint32_t tim=635;
	while(tim--);	
}

/**
 * 名称         : delay_60us(void)
 * 创建日期     : 
 * 作者         : 
 * 功能         : 60微秒延时
 * 输入参数     : 无
 * 输出参数     : 无
 * 返回结果     : 无
 * 注意和说明   : 无
 * 修改内容     : 无 
 */
void delay_60us(void)
{
	uint32_t tim=765;
	while(tim--);	
}


/**
 * 名称         : DS18B20_IO_OUT()	
 * 创建日期     : 
 * 作者         : 
 * 功能         : 设置ds18b20管脚为输出模式
 * 输入参数     : 无
 * 输出参数     : 无
 * 返回结果     : 无
 * 注意和说明   : IO方向设置
 * 修改内容     : 无 
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
 * 名称         : S18B20_IO_IN()	
 * 创建日期     : 
 * 作者         : 
 * 功能         : 设置ds18b20管脚为输入模式
 * 输入参数     : 无
 * 输出参数     : 无
 * 返回结果     : 无
 * 注意和说明   : IO方向设置
 * 修改内容     : 无 
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


/**
 * 名称         : DS18B20_DQ_OUT(uint8_t out)	
 * 创建日期     : 
 * 作者         : 
 * 功能         : 设置ds18b20管脚的输出状态
 * 输入参数     : 1	输出高电平		其他 输出低电平
 * 输出参数     : 无
 * 返回结果     : 无
 * 注意和说明   : 无
 * 修改内容     : 无 
 */
void DS18B20_DQ_OUT(uint8_t out)
{
	if(1 == out)
		HAL_GPIO_WritePin(DS18B20_GPIOX, DS18B20_GPIO_PINX,GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(DS18B20_GPIOX, DS18B20_GPIO_PINX,GPIO_PIN_RESET);
}



/**
 * 名称         : DS18B20_Rst(void)	
 * 创建日期     : 
 * 作者         : 
 * 功能         : 复位DS18B20
 * 输入参数     : 无
 * 输出参数     : 无
 * 返回结果     : 无
 * 注意和说明   : 无
 * 修改内容     : 无 
 */
void DS18B20_Rst(void)	   
{                 
	DS18B20_IO_OUT();   	//设置为输出
	DS18B20_DQ_OUT(0);  	//拉低DQ
	delay_750us();
	DS18B20_DQ_OUT(1);  	//DQ=1 
	delay_15us();
}



/**
 * 名称         : DS18B20_Check(void)   
 * 创建日期     : 
 * 作者         : 
 * 功能         : 等待DS18B20的回应
 * 输入参数     : 无
 * 输出参数     : 无
 * 返回结果     : 1:未检测到DS18B20的存在  0:存在
 * 注意和说明   : 无
 * 修改内容     : 无 
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



/**
 * 名称         : DS18B20_Read_Bit(void)     
 * 创建日期     : 
 * 作者         : 
 * 功能         : 从DS18B20读取一个位
 * 输入参数     : 无
 * 输出参数     : 无
 * 返回结果     : 1/0
 * 注意和说明   : 无
 * 修改内容     : 无 
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



/**
 * 名称         : DS18B20_Read_Byte(void)     
 * 创建日期     : 
 * 作者         : 
 * 功能         : 从DS18B20读取一个字节
 * 输入参数     : 无
 * 输出参数     : 无
 * 返回结果     : 读到的数据
 * 注意和说明   : 无
 * 修改内容     : 无 
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
 * 名称         : DS18B20_Write_Byte(uint8_t dat)     
 * 创建日期     : 
 * 作者         : 
 * 功能         : 写一个字节到DS18B20
 * 输入参数     : dat：要写入的字节
 * 输出参数     : 无
 * 返回结果     : 无
 * 注意和说明   : 无
 * 修改内容     : 无 
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
 * 名称         : DS18B20_Start(void)
 * 创建日期     : 
 * 作者         : 
 * 功能         : 开始温度转换
 * 输入参数     : 无
 * 输出参数     : 无
 * 返回结果     : 无
 * 注意和说明   : 无
 * 修改内容     : 无 
 */
void DS18B20_Start(void)
{   						               
    DS18B20_Rst();	   
    DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0x44);// convert
}


/**
 * 名称         : DS18B20_Init(void)
 * 创建日期     : 
 * 作者         : 
 * 功能         : 初始化DS18B20的IO口 DQ 同时检测DS的存在
 * 输入参数     : 无
 * 输出参数     : 无
 * 返回结果     : 1:不存在 0:存在
 * 注意和说明   : 无
 * 修改内容     : 无 
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


/**
 * 名称         : DS18B20_Get_Temp(void)
 * 创建日期     : 
 * 作者         : 
 * 功能         : 获取温度值
 * 输入参数     : 无
 * 输出参数     : 无
 * 返回结果     : 温度值 （-550~1250）
 * 注意和说明   : 无
 * 修改内容     : 无 
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
