#ifndef _ALGORITHM_H_
#define _ALGORITHM_H_

#include "stm32l0xx.h"

void ArrayMerge(uint8_t A1[], uint8_t A2[], uint8_t A[], uint8_t Size1, uint8_t Size2);
void SortArrayExtreme(uint32_t Array[], const uint32_t ArraySize,
                      const uint32_t SortHeadSize, const uint32_t SortTailSize);
uint32_t GetDelExtremeAndAverage(uint32_t Array[], const uint32_t ArraySize,
                            const uint32_t SortHeadSize, const uint32_t SortTailSize);
long hextodec_fun(unsigned char *s);
uint32_t ArrToHex(uint8_t *pData);
float HexToFloat(uint8_t *pData);
void floatToHexArray(float fSource, uint8_t *pu8Temp, long num);
uint32_t HexToUlong(uint8_t *pData);
int get_slop(int *bufx, int *bufy, uint8_t len);
uint32_t Get_Min_Max(uint32_t *dBuf, uint8_t len, uint8_t mode);
void xz_dump_on(const char *data, short size);

#endif
