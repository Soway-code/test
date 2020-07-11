
#ifndef _MODBUS_H
#define _MODBUS_H

#include "main.h"

// modbus ���ܺ�
#define MDB_F03_READ_HOLD_REG           0x03                                    // �����ּĴ���            
#define MDB_F04_READ_INPUT_REG          0x04                                    // ������Ĵ��� 
#define MDB_F05_EXT_WRITE_COIL_REG      0x05                                    // д��Ȧ�Ĵ��� �궨����������
#define MDB_F10_WRITE_INPUT_REG         0x10                                    // д���ּĴ��� 
#define MDB_F25_INT_WRITE_COIL_REG      0x25                                    // �ڲ�д��Ȧ�Ĵ��� �궨����������
#define MDB_F26_INT_READ_HOLD_REG     	0x26                                    // �ڲ������ּĴ��� 
#define MDB_F27_INT_WRITE_HOLD_REG		  0x27                                    // �ڲ�д���ּĴ��� 

// modbus ���ش�����
#define MDB_NO_ERROR                    0x00                                    // �޴���
#define MDB_FNO_INVALID                 0x01                                    // ��֧�ֵĹ�����
#define MDB_REG_ADDR_INVALID            0x02                                    // �Ĵ�����ַ����ȷ
#define MDB_REG_VALUE_OVERRIDE          0x03                                    // �Ĵ���ֵ������Χ
#define MDB_MSG_FRAME_CHECK_ERROR		    0x10									                  // CRC/LRCУ�����
#define MDB_MSG_PARITY_ERROR			      0x11									                  // ��żУ�����
#define MDB_MSG_LEN_ERROR				        0x12									                  // ֡���ȴ���
#define MDB_MSG_ADDR_ERROR				      0x13									                  // ��ַ����

// modbus �Ĵ�����ַ
#define ACC_ADDR_0x02                   0x02                                    // X��Ĵ�����ֵַ
#define ACC_ADDR_0x04                   0x04                                    // Y��Ĵ�����ֵַ
#define ACC_ADDR_0x06                   0x06                                    // Z��Ĵ�����ֵַ
#define SLAVE_ADDR_0x30                 0x30                                    // �ӻ���ַ�Ĵ�����ֵַ
#define BAUD_ADDR_0x31                  0x31                                    // �����ʼĴ�����ֵַ
#define PARITY_ADDR_0x32                0x32                                    // ��żУ��Ĵ�����ֵַ
#define COMPENSATION_ADDR_0x34          0x34                                    // ����ʹ�ܼĴ�����ֵַ
#define FREQUENCY_ADDR_0x3F          	  0x3F                                    // λ�ƺ��¶Ȳ���ֵ����Ƶ�ʵ�ֵַ
#define UPGRADE_ADDR_0x40				        0x40									                  // ϵͳ���������Ĵ�����ֵַ
#define	SETLOGICALZERO_ADDR_0x50		    0x50									                  // �����߼����Ĵ��ֵַ
#define	RESET_ADDR_0x51					        0x51									                  // �ָ��������üĴ��ֵַ
#define OUT_PUT_MOD_ADDR_0x60           0x60                                    // ����Ĵ�����ֵַ
#define PosRS_ADDR_0x88					        0x88									                  // λ�Ʊ궨/����Ĵ�����ֵַ
#define PosRangeRS_ADDR_0x89			      0x89									                  // �趨λ�����/������
#define TempRS_ADDR_0x9A				        0x9A									                  // �¶ȱ궨/����Ĵ�����ֵַ
#define TempRangeRS_ADDR_0x9B			      0x9B									                  // �趨�¶����/������
#define CompenRS_ADDR_0xCD				      0xCD									                  // �����궨���

// modbus F26���������ַ��־
#define ReadPos							0x01									// ��λ�Ʋ�����־
#define ReadTemp						0x02									// ���¶Ȳ�����־
#define ReadCompen						0x03									// ������������־
#define ReadTestData					0x04									// �����ڲ����ݱ�־

// modbus F26���������ַ����ʼ�ͽ�����ֵַ
#define PosStartAddr					0x80									// modbus F26������λ�Ʋ�����ʼ��ַ
#define PosEndAddr						0x87									// modbus F26������λ�Ʋ���������ַ
#define TempStartAddr					0x90									// modbus F26������λ���¶���ʼ��ַ
#define TempEndAddr						0x99									// modbus F26������λ���¶Ƚ�����ַ
#define CompenStartAddr					0xA0									// modbus F26������λ�Ʋ�����ʼ��ַ
#define CompenEndAddr					0xCD									// modbus F26������λ�Ʋ���������ַ
#define	TestDataAddr					0xF0									// modbus F26�������ڲ����ݿ�ʼ��ַ �ڲ����ݼĴ����Ϊ6 ��ȡʱ����һ�ζ�6������ 

// modbus F27������д��ַ��־
#define WriteTsc						0x01									// ��λ�Ʋ�����־
#define WriteTemp						0x02									// ���¶Ȳ�����־
#define WriteCompen						0x03									// ������������־

// modbus д���
#define MDBWriteNone					0x0000									// ���κ�д�������
#define MDBWriteBaudOrParity			0x0001									// д�����ʻ���żУ����
#define MDBWriteOutPutMod				0x0002									// д���ģʽ���
#define MDBWriteFrequency				0x0003									// д����Ƶ�ʱ��
#define MDBWriteUpgrade					0x0004									// дϵͳ�������
#define MDBWriteReset					0x0005									// д�ָ��������ñ��

uint8_t MDB_Get_Slave_Addr(uint8_t *master_request_data);						// ��ȡ�ӻ���ַ
uint8_t MDB_Function(void);														// ���ܲ�������
uint8_t FrameCheck(void);														// ֡У����
uint8_t MDB_F03_Broadcast_Read_Slave_Addr(void);								// �㲥����ַ
uint8_t MDB_F03_Read_Hold_Reg(void);											// �����ּĴ���
uint8_t MDB_F03_Read_Addr_Baud_Parity(void);									// ���ӻ���ַ ������ ��żУ�鵥��������������
uint8_t MDB_F03_Read_Single_Hold_Reg(void);										// ���������ּĴ���
uint8_t MDB_F04_Read_Input_Reg(void);											// ������Ĵ��� 
uint8_t MDB_F05_Write_Coil_Reg(void);											// д��Ȧ �궨����
uint8_t MDB_F05_Write_SetLogicalZero(void);										// д�߼������Ȧ
uint8_t MDB_F05_Write_Reset(void);												// ���ݱ���/�ָ���������
uint8_t MDB_F10_Write_Hold_Reg(void);											// д���ּĴ���
uint8_t MDB_F10_Write_Addr_Baud_Parity(void);									// д�ӻ���ַ ������ ��żУ�鵥��������������
uint8_t MDB_F10_Write_Single_Hold_Reg(void);									// д�������ּĴ���
uint8_t MDB_F25_Int_Write_Coil_Reg(void);										// �ڲ�д��Ȧ �궨����
uint8_t MDB_F25_Int_Write_Upgrade(void);										// д����������־��Ȧ
uint8_t MDB_F25_Int_Write_PosRS(void);											// �ڲ�λ�Ʊ궨����/ʧ��/ʹ��
uint8_t MDB_F25_Int_Write_PosRangeRS(void);										// �ڲ��궨λ������������
uint8_t MDB_F25_Int_Write_TempRS(void);											// �ڲ��¶ȱ궨ʹ��/����
uint8_t MDB_F25_Int_Write_TempRangeRS(void);									// �ڲ��궨�¶�����������
uint8_t MDB_F25_Int_Write_CompenRS(void);										// �����궨���
uint8_t MDB_F26_Int_Read_Hold_Reg(void);										// �ڲ������ּĴ���
uint8_t MDB_F26_Int_Read_Pos_Reg(uint16_t StartAddr);							// �ڲ���λ�Ʊ��ּĴ���
uint8_t MDB_F26_Int_Read_Temp_Reg(uint16_t StartAddr);							// �ڲ����¶ȱ��ּĴ���
uint8_t MDB_F26_Int_Read_Compen_Reg(uint16_t StartAddr);						// �ڲ����������ּĴ���
uint8_t MDB_F26_Int_Read_Test_Data(uint16_t StartAddr);							// �ڲ�����������
uint8_t MDB_F27_Int_Write_Hold_Reg(void);										// �ڲ�д���ּĴ���
uint8_t MDB_F27_Int_Write_Pos_Reg(uint16_t StartAddr);							// �ڲ�дλ�Ʊ��ּĴ���
uint8_t MDB_F27_Int_Write_Temp_Reg(uint16_t StartAddr);							// �ڲ�д�¶ȱ��ּĴ���
uint8_t MDB_F27_Int_Write_Compen_Reg(uint16_t StartAddr);						// �ڲ�д�������ּĴ���
void WriteCurParaToFlash(void);													// �ѵ�ǰ����д��Flash ������������
void BackupCurParaToFlash(void);												// �ѵ�ǰ�������浽��ǰ�������ͱ�����
void MDB_Snd_Msg(uint8_t *pMsg, uint8_t len);									// ������Ϣ
void WriteParaInit(void);														// Flashд���µĲ���ʱ ������Ҫ���������³�ʼ��
#endif

