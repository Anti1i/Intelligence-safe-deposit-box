#ifndef __HARDWARE_CONFIG_H
#define __HARDWARE_CONFIG_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#include "stm32f4xx_hal_tim.h"

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim3;

void System_Hardware_Init(void);

#endif
