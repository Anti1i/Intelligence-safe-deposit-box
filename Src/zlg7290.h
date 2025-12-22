#ifndef __ZLG7290_H
#define __ZLG7290_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"

void I2C_ZLG7290_Read(I2C_HandleTypeDef *I2Cx, uint8_t DevAddress, uint8_t MemAddress, uint8_t *pData, uint16_t Size);
void I2C_ZLG7290_Write(I2C_HandleTypeDef *I2Cx, uint8_t DevAddress, uint8_t MemAddress, uint8_t *pData, uint16_t Size);

#endif
