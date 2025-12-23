#ifndef __DC_MOTOR_H
#define __DC_MOTOR_H

#include "stm32f4xx_hal.h"

/* 电机驱动芯片I2C地址 */
#define DC_MOTOR_ADDR  0x9A

/* 电机使能引脚 - PE2 */
#define DC_Motor_Port  GPIOE
#define DC_Motor_Pin   GPIO_PIN_2

#define DC_Motor_Pin_Low()   HAL_GPIO_WritePin(DC_Motor_Port, DC_Motor_Pin, GPIO_PIN_RESET) 
#define DC_Motor_Pin_High()  HAL_GPIO_WritePin(DC_Motor_Port, DC_Motor_Pin, GPIO_PIN_SET)

/* 电机状态定义 */
#define MOTOR_STOP      0
#define MOTOR_FORWARD   1   // 正转（开锁）
#define MOTOR_BACKWARD  2   // 反转（关锁）

/* 函数声明 */
void DC_Motor_Control(uint8_t state);
void I2C_DC_Motor_WriteOneByte(I2C_HandleTypeDef *I2Cx, uint8_t I2C_Addr, 
                                uint8_t addr, uint8_t value);
void I2C_DC_Motor_Write(I2C_HandleTypeDef *I2Cx, uint8_t I2C_Addr, 
                        uint8_t addr, uint8_t *buf, uint8_t num);

#endif
