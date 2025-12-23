#include "stm32f4xx_hal.h"
#include "stm32f4xx_it.h"
#include "tim.h"
#include "usart.h"

/* 外部变量 */
extern TIM_HandleTypeDef htim3;
extern UART_HandleTypeDef huart1;

/******************************************************************************/
/* Cortex-M4 Processor Interruption and Exception Handlers                   */
/******************************************************************************/

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
    HAL_IncTick();
    HAL_SYSTICK_IRQHandler();  // 调用这个才会触发 HAL_SYSTICK_Callback
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line[9:5] interrupts.
  */
void EXTI9_5_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  */
void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

/**
  * @brief This function handles TIM3 global interrupt.
  */
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim3);
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart1);
}

/* Note: HAL_GPIO_EXTI_Callback is defined in main.c */
