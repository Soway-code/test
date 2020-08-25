/**@file        ds18b20.c
* @brief        ds18b20模块的应用
* @details      ds18b20温度数据获取与处理
* @author       马灿林
* @date         2020-08-01
* @version      V1.0.0
* @copyright    2020-2030,深圳市信为科技发展有限公司
**********************************************************************************
* @par 修改日志:
* <table>
* <tr><th>Date        <th>Version  <th>Author    <th>Description
* <tr><td>2020/08/01  <td>1.0.0    <td>马灿林    <td>创建初始版本
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
	PRIMASK = 0; 					//PRIMASK 初始值为 0,表示没有关中断 

#endif

/**
* @brief 配置管脚为推挽输出模式
*/
void DS18B20_IO_OUT()
{
	GPIO_InitTypeDef GPIO_Initure;
   // RCC_GPIOX_CLK;									///开启GPIOA时钟
	
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
   // RCC_GPIOX_CLK;									//开启GPIOA时钟
	
    GPIO_Initure.Pin=DS18B20_GPIO_PINX;     			//PA15
    GPIO_Initure.Mode=GPIO_MODE_INPUT;  				//浮空输入
    GPIO_Initure.Pull=GPIO_PULLUP;          			//上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;    		//高速
    HAL_GPIO_Init(DS18B20_GPIOX,&GPIO_Initure);     	//初始化
}


/**@brief       设置ds18b20管脚的输出状态
* @param[in]    out : 输出状态
* - None
* @note         out: 1	(输出高电平)	其他 (输出低电平)
*/
void DS18B20_DQ_OUT(uint8_t out)
{
	if(1 == out)
		HAL_GPIO_WritePin(DS18B20_GPIOX, DS18B20_GPIO_PINX,GPIO_PIN_SET);		//输出高电平
	else
		HAL_GPIO_WritePin(DS18B20_GPIOX, DS18B20_GPIO_PINX,GPIO_PIN_RESET);		//输出低电平
}

/**
* @brief 复位DS18B20
*/
void DS18B20_Rst(void)	   
{                 
	DS18B20_IO_OUT();   	//设置为输出
	DS18B20_DQ_OUT(0);  	//拉低DQ
	delay_tim_750us();		//延时750us 480~960us
	DS18B20_DQ_OUT(1);  	//拉高DQ 
	delay_tim_15us();		//等待从机响应时间 15~60us
}

/**@brief       等待DS18B20的回应
* @return       函数执行结果
* - 返回结果    : 1 (未检测到DS18B20的存在)  0 (存在)
*/
uint8_t DS18B20_Check(void) 	   
{   
	uint8_t retry=0;
	DS18B20_IO_IN();    				//设置为输入
    while (DS18B20_DQ_IN&&retry<160)	//等待低电平脉冲到来时间 15~60us
	{
		retry++;
		delay_tim_1us();	
	};	 
	if(retry>=160)
		return 1;
	else 
		retry=0;
	
    while (!DS18B20_DQ_IN&&retry<200)	//低电平脉冲持续时间 60~240us
	{
		retry++;
		delay_tim_1us();
	};
	if(retry>=200)
		return 1;	    
	return 0;
}

/**@brief       从DS18B20读取一个位
* @return       函数执行结果,读取到的Bit 1 / 0 
*/
uint8_t DS18B20_Read_Bit(void) 
{
	uint8_t data;
	DS18B20_IO_OUT();   		//将DQ引脚设置为推挽输出模式
	DS18B20_DQ_OUT(0); 			//拉低DQ 
	delay_tim_2us();			//进行读数据操作前，需由主机产生不超过15us的低电平脉冲
	DS18B20_DQ_OUT(1); 
								
	DS18B20_IO_IN();   		 	//设置为输入
	delay_tim_5us();
	delay_tim_5us();
	if(DS18B20_DQ_IN)
		data=1;
	else
		data=0;	
	
	delay_tim_15us();			//45us整个读数据时间（60~120us）减去前面低电平脉冲时间 
	delay_tim_15us();
	delay_tim_15us();
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
    DS18B20_IO_OUT();			//设置为输出
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if(testb)       		// 写1
        {
            DS18B20_DQ_OUT(0);	//拉低DQ 
			delay_tim_2us();	//写入1前 需要先拉低总线 1~15us        
            DS18B20_DQ_OUT(1);	//拉高DQ 
			delay_tim_60us();            
        }
        else           			 //写0
        {
            DS18B20_DQ_OUT(0);
            delay_tim_60us();            
            DS18B20_DQ_OUT(1);
			delay_tim_2us();	//恢复到下一个周期 需重信拉高总线 1us~无穷              
        }
		
	}
}
 
/**
* @brief 开始温度转换
*/
void DS18B20_Start(void)
{   						               
    DS18B20_Rst();	   			//复位ds18b20传感器
    DS18B20_Check();	 		//检测DS18B20的存在
    DS18B20_Write_Byte(0xcc);	// skip rom
    DS18B20_Write_Byte(0x44);	// convert
}

/**@brief       初始化DS18B20的IO口DQ  同时检测DS的存在
* @return       DS18B20是否存在	1 (不存在) 0 (存在);	返回0x10 获取HCLK频率失败;
*/
uint8_t DS18B20_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
    RCC_GPIOX_CLK;									//开启GPIOA时钟
	
    GPIO_Initure.Pin=DS18B20_GPIO_PINX;     		//PA15
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  		//推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          		//上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;     	//高速
    HAL_GPIO_Init(DS18B20_GPIOX,&GPIO_Initure);     //初始化
	if( 0 == Get_HCLKFreq() )						//获取HCLK的频率,频率正确返回频率值，失败返回值为 0 
		return 0x10;
	//printf("RCC=%d\n",Get_HCLKFreq());
	MX_TIMx_Init();									//初始化定时器3
	DS18B20_Rst();									//复位ds18b20传感器
	return DS18B20_Check();							//检测DS18B20的存在
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
	
#if 1 == USING_RT_THREAD_OS	   
	rt_hw_interrupt_disable(); 	// 进入临界区关中断,PRIMASK = 1	
#endif
	
	DS18B20_Start ();           //开始转换
    DS18B20_Rst();				//复位ds18b20传感器
    DS18B20_Check();	 		//检测DS18B20的存在
    DS18B20_Write_Byte(0xcc);   // skip rom
    DS18B20_Write_Byte(0xbe);   // convert	    
    TL=DS18B20_Read_Byte();     // LSB   
    TH=DS18B20_Read_Byte();     // MSB  

#if 1 == USING_RT_THREAD_OS	 	
	rt_hw_interrupt_enable(); 	//退出临界区开中断,PRIMASK = 0 
#endif
    
	if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;					//温度为负  
    }
	else 
	{	
		temp=1;					//温度为正
	}
	
    tem=TH; 					//获得高八位
    tem<<=8;    
    tem+=TL;					//获得底八位
    tem=(double)tem*0.625;		//转换     
	
	if(temp)
		return tem; 			//返回温度值
	else 
		return -tem;    
}
