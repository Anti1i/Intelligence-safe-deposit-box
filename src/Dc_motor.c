#include "Dc_motor.h"
#include "i2c.h"

#define I2C_TIMEOUT  ((uint32_t)0xFFFF)

/**
 * @brief  I2C写一个字节到电机驱动
 */
void I2C_DC_Motor_WriteOneByte(I2C_HandleTypeDef *I2Cx, uint8_t I2C_Addr, 
                                uint8_t addr, uint8_t value)
{   
    while(HAL_I2C_Mem_Write(I2Cx, I2C_Addr, addr, I2C_MEMADD_SIZE_8BIT, 
                            &value, 0x01, I2C_TIMEOUT) != HAL_OK) {};
}

/**
 * @brief  I2C写多个字节到电机驱动
 */
void I2C_DC_Motor_Write(I2C_HandleTypeDef *I2Cx, uint8_t I2C_Addr, 
                        uint8_t addr, uint8_t *buf, uint8_t num)
{
    while(num--)
    {
        I2C_DC_Motor_WriteOneByte(I2Cx, I2C_Addr, addr++, *buf++);
        HAL_Delay(5);
    }
}

/**
 * @brief  电机控制 - 支持正转、反转、停止
 * @param  state: MOTOR_STOP, MOTOR_FORWARD, MOTOR_BACKWARD
 */
void DC_Motor_Control(uint8_t state)
{
    uint8_t cmd_forward = 0xFF;
    uint8_t cmd_stop = 0x00;
    
    switch(state)
    {
        case MOTOR_FORWARD:  // 正转（开锁）
            DC_Motor_Pin_Low();  // 使能电机（PE2 = 0）
            HAL_Delay(10);
            // 向电机驱动芯片发送正转命令
            I2C_DC_Motor_Write(&hi2c1, DC_MOTOR_ADDR, 0x0F, &cmd_forward, 1);
            break;
            
        case MOTOR_BACKWARD:  // 反转（关锁）
            DC_Motor_Pin_Low();  // 使能电机（PE2 = 0）
            HAL_Delay(10);
            // 向电机驱动芯片发送反转命令
            I2C_DC_Motor_Write(&hi2c1, DC_MOTOR_ADDR, 0x03, &cmd_forward, 1);
            break;
            
        case MOTOR_STOP:  // 停止
        default:
            // 先停止电机
            I2C_DC_Motor_Write(&hi2c1, DC_MOTOR_ADDR, 0x00, &cmd_stop, 1);
            HAL_Delay(10);
            // 再禁能（PE2 = 0，根据你的硬件可能需要改成1）
            DC_Motor_Pin_Low();
            break;
    }
}
