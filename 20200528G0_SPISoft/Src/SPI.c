
#include "SPI.h"

void Delay_us(uint32_t us)
{
	uint32_t i = 0;
	for(i = 0; i < 6; i++);
}

void SPI_Send_8Bit(uint8_t sBit8)
{
	uint8_t si;
	uint8_t Bit8;
	Bit8 = sBit8;

	for(si = 0; si < 8; si++)
	{
		SPI_SCK_CLR;
		Delay_us(2);
		if(Bit8 & 0x80)
		{
			SPI_Bit_Set();
		}
		else
		{
			SPI_Bit_Clr();
		}

		Bit8 <<= 1;
	}
}

uint8_t SPI_Receive_8Bit(void)
{
	uint8_t ri;
	uint8_t rcvdata;
	rcvdata = 0x00;

	SPI_SCK_CLR;
	Delay_us(2);
	for(ri = 8; ri > 0; ri--)
	{
		rcvdata <<= 1;
		SPI_SCK_SET;
		Delay_us(1);

		if(SPI_MISO_STA)
		{
			rcvdata |= 0x01;
		}

		SPI_SCK_CLR;
		Delay_us(2);
	}

	return rcvdata;
}

void SPI_Bit_Set(void)
{
	SPI_MOSI_SET;
	Delay_us(1);
	SPI_SCK_SET;
	Delay_us(1);
	SPI_SCK_CLR;
	Delay_us(1);
}

void SPI_Bit_Clr(void)
{
	SPI_MOSI_CLR;
	Delay_us(1);
	SPI_SCK_SET;
	Delay_us(1);
	SPI_SCK_CLR;
	Delay_us(1);
}

///************************************************
//函数名称 ： SPI_WriteByte
//功    能 ： SPI写一字节数据
//参    数 ： TxData --- 发送的字节数据
//返 回 值 ： 无
//作    者 ： strongerHuang
//*************************************************/
//void SPI_WriteByte(uint8_t TxData)
//{
//  uint8_t cnt;

//  for(cnt=0; cnt<8; cnt++)
//  {
//    SPI_SCK_CLR;                                 //时钟 - 低
//    Delay_us(1);

//    if(TxData & 0x80)                            //发送数据
//      SPI_MOSI_SET;
//    else
//      SPI_MOSI_CLR;
//    TxData <<= 1;

//    Delay_us(1);
//    SPI_SCK_SET;                                //时钟 - 高
//    Delay_us(1);
//  }
//}

///************************************************
//函数名称 ： SPI_ReadByte
//功    能 ： SPI读一字节数据
//参    数 ： 无
//返 回 值 ： 读回来的字节数据
//作    者 ： strongerHuang
//*************************************************/
//uint8_t SPI_ReadByte(void)
//{
//  uint8_t cnt;
//  uint8_t RxData = 0;

//  for(cnt=0; cnt<8; cnt++)
//  {
//    SPI_SCK_CLR;                                 //时钟 - 低
//    Delay_us(1);

//    RxData <<= 1;
//    if(SPI_MISO_STA)                            //读取数据
//    {
//      RxData |= 0x01;
//    }

//    SPI_SCK_SET;                                 //时钟 - 高
//    Delay_us(1);
//  }

//  return RxData;
//}

//uint8_t SPI_RW(uint8_t data)
//{
//  uint8_t i;
//  uint8_t flag;
//  //ADXL375_SDI_0; 
//  for(i=0;i<8;i++) // output 8-bit
//  {
//    
//    if(data & 0x80)
//    {
//      SPI_MOSI_SET;         // output 'uchar', MSB to MOSI
//    }
//    else
//    {
//      SPI_MOSI_CLR; 
//    }	
//    data <<= 1;            // shift next bit into MSB..
//    //************注意：读取ADXL375_SDO时，ADXL375_SCK是先低后高，如下所示：****************//
//    SPI_SCK_SET;                // Set SCK high..
//	Delay_us(1);
//    flag = SPI_MISO_STA;
//    if(flag) // 拉高SCK，nRF24L01从MOSI读入1位数据，同时从MISO输出1位数据
//    {
//      data|=0x01;       	// 读MISO到byte最低位
//    }
//    SPI_SCK_CLR;              // ..then set SCK low again
//	Delay_us(1);
//  }
//  return(data);            // 返回读出的一字节
//}

//uint8_t SOFT_SPI_RW(uint8_t byte)
//{
//    uint8_t i,Temp=0;

//    SPI_SCK_CLR;
//    Delay_us(1);
//    for(i=0;i<8;i++)     // 循环8次
//   {
//       SPI_SCK_SET;     //拉高时钟
//       if(byte&0x80)
//        {
//          SPI_MOSI_SET;  //若最到位为高，则输出高
//        }
//       else      
//        {
//           SPI_MOSI_CLR;   //若最到位为低，则输出低
//         }
//		Delay_us(1);
//          byte <<= 1;     // 低一位移位到最高位
//          SPI_SCK_CLR;     //拉低时钟
//         Temp <<= 1;     //数据左移

//         if(SPI_MISO_STA)
//           Temp++;     //若从从机接收到高电平，数据自加一

//          SPI_SCK_SET;     //拉高时钟
//       }
//      return (Temp);     //返回数据
//}

////软件模拟SPI写(发送)
//void Soft_SPI_Write(uint8_t a)
//{
//	uint8_t cnt;
//	
//	for(cnt=0;cnt<8;cnt++)
//	{
//		SPI_SCK_SET;//拉低CLK
//		Delay_us(1);//这个延时时间任意，但要大于芯片数据手册上的(纳秒级的)
//		if(a &0x80)
//		{
//			SPI_MOSI_SET;
//		}
//		else
//		{
//			SPI_MOSI_CLR;
//		}
//		a <<= 1;
//		Delay_us(1);
//		SPI_SCK_CLR;//拉高CLK
//		Delay_us(1);
//	}
//}

////软件模拟SPI读(接收)
//uint8_t Soft_SPI_Read(void)
//{
//	uint8_t cnt;
//	uint8_t Rxdata = 0;
//	
//	for(cnt=0;cnt<8;cnt++)
//	{
//		SPI_SCK_SET;//拉低CLK
//		Delay_us(1);
//		Rxdata <<= 1;
//		if(SPI_MISO_STA)
//		{
//			Rxdata |= 0x01;
//		}
//		SPI_SCK_CLR;//拉高CLK
//		Delay_us(1);
//	}
//	return Rxdata;
//}

//uint8_t Spi_SendByte(uint8_t dat)
//{
//	uint8_t recv = 0;
//	
//	uint8_t i = 0;

////#if(SPI_MSB)
////	for(i = 8; i > 0; i--)
////#else
//	for(i = 0; i < 8; i++)
////#endif
//	{
////#if(SPI_MSB)
////		if(dat & (1<<(i-1)))
////#else
//		if(dat & (1<<i))
////#endif
//		{
//			SPI_MOSI_SET;
//		}
//		else
//		{
//			SPI_MOSI_CLR;
//		}

//		Delay_us(1);
//		SPI_SCK_SET;

////#if(SPI_MSB)
////		recv <<= 1;
////		if(SPI_MISO_STA)
////		{
////			recv |= 0x01;
////		}		
////#else
//		recv >>= 1;
//		if(SPI_MISO_STA)
//		{
//			recv |= 0x80;
//		}		
////#endif
//		
//		Delay_us(1);
//		SPI_SCK_CLR;
//	}	

//	return recv;
//}


//uint8_t SPI_SendByte(uint8_t dt)
//{
//	uint8_t i;
//	uint8_t temp=0;
//	for(i=1;i<8;i++)
//	{
//		if(dt&0x80)SPI_MOSI_SET;
//		else SPI_MOSI_CLR;
//		dt<<=1;
//		SPI_SCK_SET;
//		__nop();
//		__nop();
//		__nop();
//		temp<<=1;
//		if(SPI_MISO_STA)temp++;
//		__nop();
//		__nop();
//		__nop();
//		SPI_SCK_CLR;
//	}
//	return temp;
//}



