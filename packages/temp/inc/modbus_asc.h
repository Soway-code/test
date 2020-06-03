#ifndef __MODBUS_ASC_H
#define __MODBUS_ASC_H

#define MB_ADDRESS_BROADCAST 0x00


#define MB_EX_NONE 			(0x00)
#define MB_EX_ILLEGAL_FUNCTION 		(0x01)                                  //�Ƿ�������
#define MB_EX_ILLEGAL_DATA_ADDRESS	(0x02)                                  //�Ƿ����ݵ�ַ
#define MB_EX_ILLEGAL_DATA_VALUE	(0x03)                                  //�Ƿ�����ֵ
#define MB_EX_SLAVE_DEVICE_FAILURE	(0x04)                                  //Flashδʹ��
#define MB_EX_MEMORY_PARITY_ERROR	(0x05)                                  //У�����



/*03������ĵ�ַ��Χ*/
#define MBASC_HOLDING_REG_REGION_BGEIN		        0x30
#define MBASC_HOLDING_REG_REGION_END			0x4F


/*04������ĵ�ַ��Χ*/
#define MBASC_INPUT_REG_REGION_BGEIN        	        0x00
#define MBASC_INPUT_REG_REGION_END          	        0x04




/*10������ĵ�ַ��Χ*/
#define MBASC_MUL_REG_REGION_BGEIN          MBASC_HOLDING_REG_REGION_BGEIN
#define MBASC_MUL_REG_REGION_END            MBASC_HOLDING_REG_REGION_END




void MBASC_Function(void);
//void MBASC_AutoUpLoadFrame(void);
//void MBASC_SendMsg(uint8_t *u8Msg, uint8_t u8MsgLen);
#endif 
