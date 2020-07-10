#ifndef _COMMON_H
#define _COMMON_H

#include "stm32g0xx_hal.h"


void Delay_Ms(uint32_t cnt);
void Delay_Us(uint32_t cnt);
void delay_us(uint32_t us);
void Unshort2Array(uint16_t SourceData, uint8_t* Array);
void Unlong2Array(uint32_t SourceData, uint8_t* Array);

void long32Array(uint32_t SourceData, uint8_t* Array);
#endif






