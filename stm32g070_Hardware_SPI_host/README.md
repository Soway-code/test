#                         硬件SPI两块开发板通信练习 主机

####                                                                 



##  两块开发板（stm32G070开发板）以SPI（四种模式）的方式进行通讯

	//PA1     ------> SPI1_SCK
    	//PA2     ------> SPI1_MOSI
   	 //PA6     ------> SPI1_MISO 
	#define SPI1_SCK_PIN 	GPIO_PIN_1
	#define	SPI1_MOSI_PIN	GPIO_PIN_2	
	#define SPI1_MISO_PIN	GPIO_PIN_6
	#define SPI1_GPIOx		GPIOA
