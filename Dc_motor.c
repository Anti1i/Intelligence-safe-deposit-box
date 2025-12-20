#include "Dc_motor.h"
#include "Hardware_Config.h"

#define MOTOR_ADDR 0x9A // I2C地址

// I2C 写寄存器辅助函数
static void Motor_Write(uint8_t reg, uint8_t val)
{
    HAL_I2C_Mem_Write(&hi2c1, MOTOR_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &val, 1, 100);
}

void DC_Motor_Control(uint8_t state)
{
    switch(state)
    {
        case MOTOR_FORWARD: // 正转
            Motor_Write(0x03, 0xFF); // 假设 0x03 寄存器控制速度/方向
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET); // 使能引脚
            break;
            
        case MOTOR_BACKWARD: // 反转
            Motor_Write(0x03, 0x00);
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);
            break;
            
        case MOTOR_STOP: // 停止
            Motor_Write(0x00, 0x00);
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET); // 禁能
            break;
    }
}