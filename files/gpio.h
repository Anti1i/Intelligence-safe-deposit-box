#ifndef __GPIO_H
#define __GPIO_H

#include "stm32f4xx_hal.h"

/* 外部变量声明 */
extern uint32_t DC_Motor_Count;
extern uint8_t flag1;

void MX_GPIO_Init(void);
void Error_Handler(void);

#endif
