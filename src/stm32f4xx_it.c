#include "stm32f4xx_hal.h"
#include "stm32f4xx_it.h"
#include "RemoteInfrared.h"
#include "tim.h"
#include "usart.h"

/* 外部变量 */
extern TIM_HandleTypeDef htim3;
extern UART_HandleTypeDef huart1;
extern __IO uint32_t GlobalTimingDelay100us;

/******************************************************************************/
/* Cortex-M4 Processor Interruption and Exception Handlers                   */
/******************************************************************************/

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
    HAL_IncTick();
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line[9:5] interrupts.
  * @note  PE5 - 电机编码器中断
  */
void EXTI9_5_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
}

/**
  * @brief This function handles EXTI line[15:10] interrupts.
  * @note  PF15 - 红外接收器中断
  *        PD13 - 其他功能
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

/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin: Specifies the pins connected to the EXTI line.
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    extern uint32_t DC_Motor_Count;
    extern uint8_t flag1;
    
    if(GPIO_Pin == GPIO_PIN_5)
    {
        /* PE5 - 电机编码器脉冲 */
        DC_Motor_Count++;
    }
    
    if(GPIO_Pin == GPIO_PIN_13)
    {
        /* PD13 - 其他中断 */
        flag1 = 1;
    }
    
    if(GPIO_Pin == GPIO_PIN_15)
    {
        /* PF15 - 红外接收器 */
        Remote_Infrared_KEY_ISR();
    }
}
