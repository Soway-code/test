#ifndef __PCAP_H_
#define __PCAP_H_

//REG ADDR
#define TEST_REG_ADDR   0x0000
#define SRAM_CMD_WR     0x9000
#define SRAM_CMD_RD     0x1000

#define SRAM_DATA_NUM   944
#define REG_CONF_NUM     21

//command 
//8位
#define PCAP_ALL_RESET     0x88             //复位所有的状态
#define PCAP_PTL_RESET 0x8A             //复位重要部分的状态
#define PCAP_MEASURE   0x8C             //开始一次电容测量
//16位
//#define 
extern unsigned long PCap_Status;
extern unsigned long ulWritePCAPXramErr;
extern BitAction PCap_Ready;

void spi_enable(void);
void spi_disable(void);
void spi_send_0(void);
void spi_send_1(void);
void spi_write8(unsigned char wbuf8);
void spi_write16(unsigned int wbuf16);
void spi_write32(unsigned long wbuf32);

unsigned char spi_read8(void);
unsigned long spi_read24(void);


void PCap_Reset(void);
void PCap_PTL_Reset(void);
void PCap_MEASURE(void);
void PCap_MEASURE_TEMP(void);
void PCap_Enable(void);
unsigned char PCap_Test(void);
unsigned long PCap_WR_SRAM(void);
void PCap_Reg_Config(void);

//float PCap_Res_Value(void);
unsigned long PCap_Res_Value(void);
unsigned long PCap_Res_Stau(void);
uint32_t PCap_Res_Temp(void);
unsigned char PCap_RD_SRAM_Byte(unsigned char addr);

//float FilterAdcData(float *AdcBuf,float (*f)());						//对ADC转换数据滤波
//unsigned long FilterAdcData(unsigned long *AdcBuf,unsigned long (*f)());
extern void FilterAdcData(void);
float PCap_Res_Value_test(void);
extern void PCap_Init(void);
void resetPcap(void);


#endif
