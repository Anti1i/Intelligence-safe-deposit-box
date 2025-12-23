#include "stm32f4xx_hal.h"
#include "stm32f4xx_it.h"

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
