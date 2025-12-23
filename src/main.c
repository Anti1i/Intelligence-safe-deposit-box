/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "RemoteInfrared.h"
#include "Dc_motor.h"
#include "i2c.h"
#include "tim.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
__IO uint32_t GlobalTimingDelay100us;

/* 状态定义 */
typedef enum {
    STATE_LOCKED,
    STATE_UNLOCKING,
    STATE_OPENED,
    STATE_LOCKING
} SystemState_t;

SystemState_t CurrentState = STATE_LOCKED;
extern uint32_t DC_Motor_Count;
extern uint32_t DC_Motor_Data;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

int main(void)
{
  /* USER CODE BEGIN 1 */
  uint32_t ir_raw_data = 0;
  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_TIM3_Init();

  /* USER CODE BEGIN 2 */
  HAL_Delay(500);

  /* 确保电机初始停止 */
  DC_Motor_Pin_Low();

  /* 启动定时器 */
  HAL_TIM_Base_Start_IT(&htim3);

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
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
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
        if (ir_raw_data == IR_RAW_KEY_UNLOCK)
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
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_SET);
        CurrentState = STATE_OPENED;
        printf(">>> Status: OPENED <<<\r\n\r\n");
        break;

      case STATE_OPENED:
        if (ir_raw_data == IR_RAW_KEY_LOCK)
        {
          printf("\r\n>>> LOCKING... <<<\r\n");
          CurrentState = STATE_LOCKING;
        }
        else if (ir_raw_data == IR_RAW_KEY_STOP)
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
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_RESET);
        CurrentState = STATE_LOCKED;
        printf(">>> Status: LOCKED <<<\r\n\r\n");
        printf("Motor Count=%lu, Speed Data=%lu\r\n", DC_Motor_Count, DC_Motor_Data);
        break;
    }

    HAL_Delay(10);
  }
  /* USER CODE END 3 */
}

/** System Clock Configuration - 使用HSI内部时钟（参考能跑的项目）
*/
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  __PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/10000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */
int fputc(int ch, FILE *f)
{
  while((USART1->SR&0X40)==0);  // 循环等待,直到发送完毕
  USART1->DR = (uint8_t) ch;
  return ch;
}

void HAL_SYSTICK_Callback(void)
{
  if(GlobalTimingDelay100us != 0)
  {
    GlobalTimingDelay100us--;
  }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  extern uint8_t flag1;

  if(GPIO_Pin == GPIO_PIN_5)
  {
    DC_Motor_Count++;
  }
  if(GPIO_Pin == GPIO_PIN_13)
  {
    flag1 = 1;
  }
  if(GPIO_Pin == GPIO_PIN_15)
  {
    Remote_Infrared_KEY_ISR();
  }
}

void Error_Handler(void)
{
  while(1) {}
}
/* USER CODE END 4 */

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
