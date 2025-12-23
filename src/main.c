/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stdio.h"
#include "RemoteInfrared.h"
#include "Dc_motor.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* 状态定义 */
typedef enum {
    STATE_LOCKED,
    STATE_UNLOCKING,
    STATE_OPENED,
    STATE_LOCKING
} SystemState_t;

SystemState_t CurrentState = STATE_LOCKED;

/* 全局变量 */
__IO uint32_t GlobalTimingDelay100us = 0;
extern uint32_t DC_Motor_Count;  // 电机编码器计数（来自gpio.c）
extern uint32_t DC_Motor_Data;   // 电机转数（来自tim.c）

/* 函数声明 */
void SystemClock_Config(void);

int main(void)
{
    uint32_t ir_raw_data = 0;

    /* MCU Configuration */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_USART1_UART_Init();
    MX_TIM3_Init();

    HAL_Delay(500);

    /* 确保电机初始停止 */
    DC_Motor_Pin_Low();  // PE2 = 0

    /* 启动定时器 */
    HAL_TIM_Base_Start_IT(&htim3);

    /* 启动信息 */
    printf("\r\n========================================\r\n");
    printf("STM32F407 IR Remote Safe System\r\n");
    printf("========================================\r\n");
    printf("Motor: STOPPED\r\n");
    printf("Status: LOCKED\r\n");
    printf("\r\n");
    printf("=== Detecting Your Remote Keys ===\r\n");
    printf("Press keys to see Raw Data values\r\n");
    printf("Then configure RemoteInfrared.h\r\n");
    printf("========================================\r\n\r\n");

    /* 蜂鸣器提示上电 */
    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_15, GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(GPIOH, GPIO_PIN_15, GPIO_PIN_RESET);

    while (1)
    {
        /* 读取红外遥控器 */
        ir_raw_data = Remote_Infrared_GetRawData();

        if (ir_raw_data != 0)
        {
            /* 蜂鸣器按键音 */
            HAL_GPIO_WritePin(GPIOH, GPIO_PIN_15, GPIO_PIN_SET);
            HAL_Delay(50);
            HAL_GPIO_WritePin(GPIOH, GPIO_PIN_15, GPIO_PIN_RESET);

            printf("Detected: Raw=0x%08X, State=%d\r\n", ir_raw_data, CurrentState);
        }

        /* 状态机 */
        switch (CurrentState)
        {
            case STATE_LOCKED:
                if (ir_raw_data == IR_RAW_KEY_UNLOCK)  // 按开锁键
                {
                    printf("\r\n>>> UNLOCKING... <<<\r\n");
                    CurrentState = STATE_UNLOCKING;
                }
                break;

            case STATE_UNLOCKING:
                printf("Motor Forward...\r\n");
                DC_Motor_Control(MOTOR_FORWARD);
                HAL_Delay(2000);
                DC_Motor_Control(MOTOR_STOP);
                printf("Motor Stopped\r\n");

                HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_SET);  // 继电器ON

                CurrentState = STATE_OPENED;
                printf(">>> Status: OPENED <<<\r\n\r\n");
                break;

            case STATE_OPENED:
                if (ir_raw_data == IR_RAW_KEY_LOCK)  // 按关锁键
                {
                    printf("\r\n>>> LOCKING... <<<\r\n");
                    CurrentState = STATE_LOCKING;
                }
                else if (ir_raw_data == IR_RAW_KEY_STOP)  // 急停
                {
                    printf("\r\n>>> EMERGENCY STOP <<<\r\n");
                    DC_Motor_Control(MOTOR_STOP);
                    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_RESET);
                }
                break;

            case STATE_LOCKING:
                printf("Motor Backward...\r\n");
                DC_Motor_Control(MOTOR_BACKWARD);
                HAL_Delay(2000);
                DC_Motor_Control(MOTOR_STOP);
                printf("Motor Stopped\r\n");

                HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_RESET);  // 继电器OFF

                CurrentState = STATE_LOCKED;
                printf(">>> Status: LOCKED <<<\r\n\r\n");
                printf("Motor Count=%lu, Speed Data=%lu\r\n", DC_Motor_Count, DC_Motor_Data);
                break;
        }

        HAL_Delay(10);
    }
}

/** System Clock Configuration - 使用你能跑的旧版本配置 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_ClkInitTypeDef RCC_ClkInitStruct;

    __PWR_CLK_ENABLE();

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
                                |RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);  // 1ms系统节拍

    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
}

void Error_Handler(void)
{
    while(1) {}
}

/* printf重定向 */
int fputc(int ch, FILE *f)
{
    uint8_t tmp[1]={0};
    tmp[0] = (uint8_t)ch;
    HAL_UART_Transmit(&huart1,tmp,1,10);
    return ch;
}
