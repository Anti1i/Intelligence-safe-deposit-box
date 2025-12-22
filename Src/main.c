#include "Hardware_Config.h"
#include "zlg7290.h"
#include "Dc_motor.h"
#include <stdio.h>

/* --- 状态定义 --- */
typedef enum {
    STATE_LOCKED,       // 锁定
    STATE_UNLOCKING,    // 正在开锁
    STATE_OPENED,       // 已开启
    STATE_LOCKING       // 正在关锁
} SystemState_t;

SystemState_t CurrentState = STATE_LOCKED;
uint8_t key_val = 0;

int main(void)
{
    /* 1. 硬件初始化 (时钟已改为 HSI，修复乱码) */
    HAL_Init();
    System_Hardware_Init(); 
    
    /* 2. 初始状态 */
    printf("\r\nSystem Boot: Keypad Safe V4.0 (HSI Clock)\r\n");
    printf("Waiting for Key Press '1' to Unlock...\r\n");
    
    // 蜂鸣器响一声提示上电
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);

    // 继电器吸合 (供电)
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_8, GPIO_PIN_SET);

    while (1)
    {
        /* --- 核心任务: 读取 ZLG7290 键盘 --- */
        // 读取 0x01 寄存器获取键值
        key_val = 0;
        I2C_ZLG7290_Read(&hi2c1, 0x71, 0x01, &key_val, 1);
        
        if (key_val != 0) {
            printf("Key Pressed: 0x%02X\r\n", key_val); // 调试用: 打印键值
            
            // 蜂鸣器按键音
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_SET);
            HAL_Delay(50);
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);
        }

        /* --- 状态机逻辑 --- */
        switch (CurrentState)
        {
            case STATE_LOCKED:
                // 0x1C 是 ZLG7290 键盘上 '1' 的常见键值
                // 如果你的板子 '1' 不是 0x1C，请看串口打印的值修改这里
                if (key_val == 0x1C) 
                {
                    printf("Password Correct! Unlocking...\r\n");
                    CurrentState = STATE_UNLOCKING;
                }
                break;

            case STATE_UNLOCKING:
                DC_Motor_Control(MOTOR_FORWARD); // 电机正转开锁
                HAL_Delay(2000);                 // 动作持续 2秒
                CurrentState = STATE_OPENED;
                printf("Status: OPENED\r\n");
                break;

            case STATE_OPENED:
                // 按 '2' (通常是 0x1B) 关锁，或者按 '1' 也行，这里设为按 '2' 关锁
                if (key_val == 0x1B) 
                {
                    printf("Command: Lock\r\n");
                    CurrentState = STATE_LOCKING;
                }
                break;

            case STATE_LOCKING:
                DC_Motor_Control(MOTOR_BACKWARD); // 电机反转关锁
                HAL_Delay(2000);
                DC_Motor_Control(MOTOR_STOP);     // 停止
                CurrentState = STATE_LOCKED;
                printf("Status: LOCKED\r\n");
                break;
        }
        
        HAL_Delay(100); // 延时防抖
    }
}
