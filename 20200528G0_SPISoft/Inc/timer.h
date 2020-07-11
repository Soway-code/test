#ifndef _TIME_H_
#define _TIME_H_

#include "main.h"
extern uint8_t Tim1Flag;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim3;
void MX_TIM1_Init(void);

void MX_TIM2_Init(void);

void MX_TIM3_Init(void);


#endif
