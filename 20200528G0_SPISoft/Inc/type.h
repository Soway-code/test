#ifndef _TYPE_H
#define _TYPE_H

#include "main.h"
// ADC
#define SamplingArrayLen						(uint16_t)1000					// λ�ƺ��¶Ȳ������鳤��
// Flashд��־λ
#define FLAG_ENABLE								0x5555							// ��־ʹ��

// �ӻ���ַ
#define BROADCAST_ADDR_0x00         0x0000							// �㲥��ַ
#define DEFAULT_ADDR_0x01						0x0001							// �ӻ���ַ(Ĭ��ֵ)

// ����ʹ��
#define COMPENSATION_ENABLE						0x0001							// ����ʹ��
#define COMPENSATION_DISABLE					0x0002							// ����ʧ��(Ĭ��ֵ)

// �����ʲ���
#define BAUDRATA_2400							  0x0001							// ������2400
#define BAUDRATA_4800							  0x0002							// ������4800
#define BAUDRATA_9600							  0x0003							// ������9600(Ĭ��ֵ)
#define BAUDRATA_19200							0x0004							// ������19200
#define BAUDRATA_38400							0x0005							// ������38400
#define BAUDRATA_57600							0x0006							// ������57600
#define BAUDRATA_115200							0x0007							// ������115200

// ��żУ�����
#define PARITY_ODD								0x0001							// ��У��
#define PARITY_EVEN								0x0002							// żУ��(Ĭ��ֵ)
#define PARITY_NONE								0x0003							// ��У��

// �ϴ�Ƶ��

#define FREQUENCY_1S							0x0001							// ���1S
#define FREQUENCY_2S							0x0002							// ���2S
#define FREQUENCY_5S							0x0003							// ���5S
#define FREQUENCY_10S							0x0004							// ���10S
#define FREQUENCY_30S							0x0005							// ���30S
#define FREQUENCY_60S							0x0006							// ���60S
#define FREQUENCY_Stop							0x0007							// ֹͣ�ϴ�

// ���ģʽ
#define OUTPUTMOD_RTU							  0x0001							// RTUģʽ(Ĭ��ֵ)
#define OUTPUTMOD_ASCII							0x0002							// ASCIIģʽ

// ����ʹ��
#define UPGRADE_DISABLE							0x00							// ������־ʧ��(Ĭ��ֵ)
#define UPGRADE_ENABLE							0xFF							// ������־ʹ��

#define SamplingUnfinish						0x00							// ����δ��ɱ��
#define SamplingFinish							0x01							// ������ɱ��


extern uint16_t FreTable[8];


typedef struct
{
	uint8_t UpgradeFlag;														// ϵͳ������־ �洢�����ַ Addr_0x08003000 
	uint8_t Addr;																    // �ӻ�ID 		�洢�����ַ Addr_0x08003001
	uint8_t Baud;																    // ������		�洢�����ַ Addr_0x08003002
	uint8_t Parity;																  // ��żУ�� 	�洢�����ַ Addr_0x08003003
	uint8_t OutPutMod;															// ���ģʽ 	�洢�����ַ Addr_0x08003004
	uint8_t Reserved[11];														// ���� �������� ������Ӹ������ ��16�ֽڶ���
}CommonPara_Typedef;															// ͨ�ò����ṹ�� ͨ�ò�����ͻ��ɲ��������ṹ�岿�ֲ����ص� ���洢��ʽ��ͬ
																				          // Ŀ����Ϊ�˼���BootLoad��Modbus�ȶ���
typedef struct
{
  uint16_t Flag;																  // Flashд��־
  uint16_t TempH;                             	  // Addr_0x02 ���ݸ�16λ 
  uint16_t TempL;                             	  // Addr_0x03 ���ݵ�16λ
  uint16_t Addr;                             		  // Addr_0x30 �豸��ַ
	uint16_t Baud;                             		  // Addr_0x31 ������
	uint16_t Parity;                            	  // Addr_0x32 ��żУ��
	uint16_t Compensation;                      	  // Addr_0x34 ����ʹ��
	uint16_t UploadFreq;                      		  // Addr_0x3F �Զ��ϴ�ʱ��
  uint16_t UpgradeFlag;								// Addr_0x40 ϵͳ������־
  uint16_t Reset;                             	  // Addr_0x51 �ָ���������
  uint16_t OutPutMod;                         	  // Addr_0x60 ���ģʽ
	uint16_t Reserved[5];													  // ���� �������� ������Ӹ������ ��16�ֶ���
}CustomerPara_Typedef;													  // �ͻ��ɲ��������ṹ��



typedef struct
{
	CommonPara_Typedef		Common;												// ͨ�ò����ṹ�� ͨ�ò�����ͻ��ɲ��������ṹ�岿�ֲ����ص� ���洢��ʽ��ͬ Ϊ����BootLoad��Modbus�淶 
	CustomerPara_Typedef	Customer;											// �ͻ��ɲ��������ṹ�� 

}FlashPara_Typedef;																// Flash��������ṹ�� �ṹ���е�Data������������

#endif


