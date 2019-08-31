/*
 * main.c
 *
 *  Created on: 23 cze 2019
 *      Author: Arkadiusz Pytlik
 *  STM8S port: Zbigniew Gibek
 * */

/*- Private include -------------------------------------*/
#include "stm8s.h"
#include "hw_delay.h"
#include "TM1637.h"


/*- Private definitions ---------------------------------*/
#ifndef F_CPU
#define F_CPU     16000000UL/* 16MHz clock               */
#endif /* F_CPU */

#define PRESCALE  1600      /* Initial TIMER divider     */
#define PERIOD    1000      /* Key scan per 100ms        */
#define AUTO_OCR  ((F_CPU / PRESCALE / PERIOD) - 1)


/*- Function prototypes ---------------------------------*/
void KeyboardCallback(uint8_t code);

static void STM8_Clock_Setup(void);
static void TIMER1_Init(void);


/*- Main Function ---------------------------------------*/
/**
  * @brief   Main loop
  * @param   None
  * @retval  None - Embedded
  */
int main(void)
{
  STM8_Clock_Setup();
  TIMER1_Init();
  hw_delay_init();

  TM_Init();
  TM_RegisterKeyboardCallback(KeyboardCallback);
  enableInterrupts();

  while(TRUE) {
    TM_Task();
  }
}


/*- Functions -------------------------------------------*/
/**
  * @brief   System clock config
  * @param   None
  * @retval  None
  */
static void STM8_Clock_Setup(void)
{
  CLK_DeInit();

  /* Enable clock source */
  CLK_HSECmd(DISABLE);
  CLK_LSICmd(DISABLE);
  CLK_HSICmd(ENABLE);
  while(CLK_GetFlagStatus(CLK_FLAG_HSIRDY) == FALSE);

  /* Set multiplexers */
  CLK_ClockSwitchCmd(     ENABLE);
  CLK_HSIPrescalerConfig( CLK_PRESCALER_HSIDIV1);
  CLK_SYSCLKConfig(       CLK_PRESCALER_CPUDIV1);

  CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO, CLK_SOURCE_HSI,
    DISABLE, CLK_CURRENTCLOCKSTATE_ENABLE);
}


/**
  * @brief   TIM1 main config
  * @param   None
  * @retval  None
  */
void TIMER1_Init(void)
{
  /* Connect clock to TIM1 */
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, ENABLE);

  /* Configure TIM1 counter and compare */
  TIM1_DeInit();
  TIM1_TimeBaseInit(PRESCALE, TIM1_COUNTERMODE_UP, AUTO_OCR, 1);

  /* Enable interrupts and go */
  TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE);
  TIM1_Cmd(ENABLE);
}


/**
  * @brief   Callback function for key events
  * @param   code  Keycode
  * @retval  None
  */
void KeyboardCallback(uint8_t code)
{
  static uint8_t cnt, dim = 8, custom = 1;

  switch(code) {
  case TM_KEYCODE_0:
    dim = 0;
    break;
  case TM_KEYCODE_1:
    dim = 1;
    break;
  case TM_KEYCODE_2:
    dim = 2;
    break;
  case TM_KEYCODE_3:
    dim = 3;
    break;
  case TM_KEYCODE_4:
    dim = 4;
    break;
  case TM_KEYCODE_5:
    dim = 5;
    break;
  case TM_KEYCODE_6:
    dim = 6;
    break;
  case TM_KEYCODE_7:
    dim = 7;
    break;
  case TM_KEYCODE_8:
    dim = 8;
    break;
  case TM_KEYCODE_9:

    break;
  case TM_KEYCODE_A:

    break;
  case TM_KEYCODE_B:
    cnt = 0; /* clear counter */
    break;
  case TM_KEYCODE_UP:
    cnt++;   /* increment sample counter */
    break;
  case TM_KEYCODE_DOWN:
    cnt--;   /* decrement sample counter */
    break;
  case TM_KEYCODE_LEFT:
    custom <<= 1;
    if(custom == 0x40) custom = 0x01;
    break;
  case TM_KEYCODE_RIGHT:
    custom >>= 1;
    if(custom == 0x00) custom = 0x20;
    break;
  case TM_KEYCODE_NONE:
    /* Key released!
     * You can add an extra code here if release of the key also should be managed
     * */
    break;
  default:

    break;
  }

  /* Display counter on the left three digits */
  TM_SetDigit(0, cnt / 100 % 10, TM_CHAR_REGULAR );
  TM_SetDigit(1, cnt / 10 % 10, TM_CHAR_REGULAR );
  TM_SetDigit(2, cnt % 10, TM_CHAR_WITH_DP );

  /* Display raw key code on the right three digits */
  TM_SetDigit(3, custom, TM_CHAR_CUSTOM );
  TM_SetDigit(4, code / 10 % 10, TM_CHAR_REGULAR );
  TM_SetDigit(5, code % 10, TM_CHAR_REGULAR );

  TM_SetDuty(dim);
}


/*- Interrupt Handlers ----------------------------------*/
/**
  * @brief   TIM1 overflow with ARR
  * @param   None
  * @retval  None
  */
void TIM1_UPD_OVF_BRK_IRQHandler() __interrupt(11)
{
  TM_ISR();
  TIM1_ClearITPendingBit(TIM1_IT_UPDATE);
}
