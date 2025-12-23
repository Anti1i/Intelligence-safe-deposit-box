#ifndef __TIM_H
#define __TIM_H

#include "stm32f4xx_hal.h"

extern TIM_HandleTypeDef htim3;
extern uint32_t DC_Motor_Data;

void MX_TIM3_Init(void);
void Error_Handler(void);

#endif
