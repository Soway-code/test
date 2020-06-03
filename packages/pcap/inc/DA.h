#ifndef _DA_H_
#define _DA_H_

typedef struct{
    BitAction bDADebugEn;
    float usDACode;
}DADBG_TypeDef;

extern DADBG_TypeDef DAOilDebug;
extern DADBG_TypeDef DAOutDebug;

void DA_Init(void);
void DA_Write(unsigned short DACode);
void DA2_Init(void);
void DA2_Write(unsigned short DACode);

#endif

