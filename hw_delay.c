/*
 * hw_delay.c
 *
 *  Created on: 7 sie 2019
 *      Author: Zbigniew Gibek
 */

#include "stm8s.h"

#include "hw_delay.h"


/**
  * @brief   TIM4 Clock enable for delay functions
  *          Timer will be reconfigured by these functions
  * @param   None
  * @retval  None
  */
void hw_delay_init(void)
{
  /* Connect clock to TIM4 for delays */
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER4, ENABLE);
}


/**
  * @brief   Delay in microseconds, in blocking mode
  * @param   us : Delay time as 16bit unsigned value
  * @retval  None
  */
void hw_delay_us(uint16_t us)
{
  TIM4_Cmd(DISABLE);
  TIM4_TimeBaseInit(TIM4_PRESCALER_1, TIM4_US_PERIOD);

  TIM4_Cmd(ENABLE);
  while(--us) {
    while(TIM4_GetFlagStatus(TIM4_FLAG_UPDATE) == RESET) {};
    TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  };
}


/**
  * @brief   Delay in milliseconds, in blocking mode
  * @param   ms : Delay time as 16bit unsigned value
  * @retval  None
  */
void hw_delay_ms(uint16_t ms)
{
  uint16_t counter;

  TIM4_Cmd(DISABLE);
  TIM4_TimeBaseInit(TIM4_PRESCALER_128, TIM4_MS_PERIOD);
  TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  counter = ms;

  TIM4_Cmd(ENABLE);
  while(--counter) {
    while(TIM4_GetFlagStatus(TIM4_FLAG_UPDATE) == RESET) {};
    TIM4_ClearFlag(TIM4_FLAG_UPDATE);
  };
}