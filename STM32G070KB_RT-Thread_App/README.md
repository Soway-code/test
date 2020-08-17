#                         DS18B20获取温度APP

####                                                             




## STM32G070KB开发板ds18b20 

通过以下三个宏定义来修改连接DS18B20的DQ管脚
#define RCC_GPIOX_CLK 		__HAL_RCC_GPIOA_CLK_ENABLE();	//开启GPIOx时钟
#define DS18B20_GPIOX 		GPIOA				//选择DS18B20 GPIOx端口
#define DS18B20_GPIO_PINX 		GPIO_PIN_15			//选择DS18B20 连接的管脚


1.DS18B20初始化函数，该函数有二个返回值分别是1,0，返回1 DS18B20不存在, 返回0 DS18B20存在初始化成功。
uint8_t DS18B20_Init(void);
    
2.DS18B20获取温度值的函数，该函数返回值是温度值的十倍 (24.5℃时返回245)，
short DS18B20_Get_Temp(void);


