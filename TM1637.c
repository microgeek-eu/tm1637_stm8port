/*
 * TM1637.c
 *
 *  Created on: 23 cze 2019
 *      Author: Arkadiusz Pytlik
 *  STM8S port: Zbigniew Gibek
 */

#include "TM1637.h"
#include "TM1637_LL.h"


#define TM_BASE_ADDR       0xC0
#define TM_WRITE_INCR      0x40
#define TM_READ_SCAN_DATA  0x42
#define TM_WRITE_FIXED     0x44

#define TM_DISPLAY_CTRL    0x80
#define TM_DISPLAY_ON      0x08


static const uint8_t digits[] =
{
    0x3f,  //0
    0x06,  //1
    0x5b,  //2
    0x4f,  //3
    0x66,  //4
    0x6d,  //5
    0x7d,  //6
    0x07,  //7
    0x7f,  //8
    0x6f,  //9
    0x77,  //10  A
    0x7C,  //11  b
    0x39,  //12  C
    0x5E,  //13  d
    0x79,  //14  E
    0x71,  //15  F
    0x00   //off
};


static volatile bool TM_scanFlag;
static OnPress TM_callback;


static void TM_start(void)
{
  TM_CLK_HIGH;
  TM_DIO_HIGH;
  DELAY_US(2);
  TM_DIO_LOW;
}


static void TM_write8bits(uint8_t byte)
{
  for(uint8_t i = 0; i < 8; ++i) {
    TM_CLK_LOW;
    DELAY_US(2);

    if(byte & 0x01) {
      TM_DIO_HIGH;
    }
    else {
      TM_DIO_LOW;
    }
    TM_CLK_HIGH;
    DELAY_US(2);

    byte >>= 1;
  }
}


static void TM_stop(void)
{
  TM_CLK_LOW;
  TM_DIO_LOW;

  DELAY_US(2);

  TM_CLK_HIGH;
  DELAY_US(2);
  TM_DIO_HIGH;
}


static void TM_ack(void)
{
  uint16_t timeout = 1000;

  TM_CLK_LOW;

  TM_DIO_IN;
  TM_DIO_LOW;

  while((TM_DIO_IS_HIGH) && (--timeout)); // thx Zbychu

  TM_DIO_OUT;
  TM_CLK_HIGH;
  DELAY_US(2);
}


static void TM_write(uint8_t *msg, uint8_t len)
{
  TM_start();

  while(len--) {
    TM_write8bits(*msg++);
    TM_ack();
  }

  TM_stop();
}


/*
 * @brief  MCU pins initialization
 * @param  none
 * @retval none
 */
void TM_Init(void)
{
  TM_DIO_HIGH;
  TM_CLK_HIGH;

  TM_DIO_OUT;
  TM_CLK_OUT;
}


/*
 * @brief  Function sets new digit on selected position.
 * @param  digit - selected digit (0 to 5)
 * @param  value - new value (0 to 16 or TM_DIGIT_VALUE_x) for
 *          selected digit or custom character bitmap made from
 *          TM_CUSTOM_SEGMENT_x elements
 * @param  character_type:
 *          @TM_CHAR_REGULAR  treats parameter value as regular
 *                            value to display (from 0h to Ah or off),
 *          @TM_CHAR_WITH_DP  treats parameter value as regular value
 *                            with decimal point enabled (from 0h to Ah
 *                            or off but DP is on),
 *          @TM_CHAR_CUSTOM   treats parameter value as custom bitmap
 *                            made from TM_CUSTOM_SEGMENT_x elements
 * @retval none
 */
void TM_SetDigit(uint8_t digit, uint8_t value, TM_DP_StateType character_type)
{
  uint8_t command = TM_WRITE_INCR;
  uint8_t msg[2];

  msg[0] = TM_BASE_ADDR + digit;

  if(character_type != TM_CHAR_CUSTOM) {
    msg[1] = digits[value]; /* Load bitmap from LUT */

    if(character_type == TM_CHAR_WITH_DP) {
      msg[1] |= (1 << 7);   /* Enable decimal point */
    }
  }
  else {
    msg[1] = value;         /* Insert raw bitmap    */
  }

  TM_write(&command, sizeof(command));
  TM_write(msg, sizeof(msg));
}


/*
 * @brief  Sets digits brightness.
 * @param  duty - from 0 (display off) to 8 (display duty cycle 14/16)
 * @retval none
 * */
void TM_SetDuty(uint8_t duty)
{
  uint8_t command;

  duty &= 0x0F;

  command = (duty) ? (TM_DISPLAY_CTRL | TM_DISPLAY_ON | (duty - 1)) : TM_DISPLAY_CTRL;

  TM_write(&command, sizeof(command));
}


/*
 * @brief  Reads keyboard state.
 * @param  none
 * @retval key code or 0x00 when no key is pressed
 */
uint8_t TM_GetKeys(void)
{
  uint8_t command = TM_READ_SCAN_DATA;
  uint8_t data = 0;

  TM_start();
  TM_write8bits(command);
  TM_ack();

  TM_DIO_IN;
  for(uint8_t i = 0; i < 8; ++i) {
    TM_CLK_LOW;
    data >>= 1;
    DELAY_US(2);
    TM_CLK_HIGH;
    DELAY_US(2);

    if(TM_DIO_IS_HIGH) {
      data |= 0x80;
    }
  }

  TM_ack();
  TM_stop();

  return (uint8_t)(~data);
}


/*
 * @brief  Automatic keyboard check & call (if registered) callback
 *         function.
 * @param  none
 * @retval none
 */
void TM_Task(void)
{
  static uint8_t previousKeyCode = TM_KEYCODE_NONE;
  uint8_t currentKeyCode;

  if(TM_scanFlag == TRUE) {
    TM_scanFlag = FALSE;
    currentKeyCode = TM_GetKeys();

    if(currentKeyCode != previousKeyCode) {
      if(TM_callback != ((void*)0)) {
        TM_callback(currentKeyCode);

        previousKeyCode = currentKeyCode;
      }
    }
  }
}


/*
 * @brief  Function which must be called periodically (ie. by interrupt
 *         subroutine) when TM_Task being used.
 * @param  none
 * @retval none
 */
void TM_ISR(void)
{
  TM_scanFlag = TRUE;
}


/*
 * @brief  Attach callback function
 * @param  pointer to the void fun(uint8_t) function prototype
 * @retval none
 */
void TM_RegisterKeyboardCallback(OnPress cb)
{
  TM_callback = cb;
}
