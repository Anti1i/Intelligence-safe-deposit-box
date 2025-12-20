#include "stm32f4xx_hal.h"
#include "stm32f4xx_it.h"
#include "RemoteInfrared.h"
#include "Hardware_Config.h"

/* 外部变量引用 */
extern TIM_HandleTypeDef htim3;
extern DMA_HandleTypeDef hdma_adc3;
extern UART_HandleTypeDef huart1;

/******************************************************************************/
/* Cortex-M4 Processor Interruption and Exception Handlers          */ 
/******************************************************************************/
void SysTick_Handler(void)
{
  HAL_IncTick();
}

/******************************************************************************/
/* Peripheral Interrupt Handlers                        */
/******************************************************************************/

/* 红外需要的高精度计时基准 (100us) */
void TIM3_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim3);
  Remote_Infrared_TimeTick(); // 红外计时递减
}

/* 红外信号的边沿检测 */
void EXTI15_10_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13); // 假设红外接在 PD13 (根据 gpio.c)
}

// 串口中断 (用于远程控制/调试)
void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart1);
}

// DMA中断 (ADC采集完成)
void DMA2_Stream0_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_adc3);
}