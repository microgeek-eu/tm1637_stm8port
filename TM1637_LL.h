/*
 * TM1637_LL.h
 *
 *  Created on: 29 cze 2019
 *      Author: Arkadiusz Pytlik
 *  STM8S port: Zbigniew Gibek
 */

#ifndef TM1637_LL_H_
#define TM1637_LL_H_


#include "stm8s.h"
#include "hw_delay.h"


#define TM_DIO_PORT   (GPIOB->ODR)
#define TM_DIO_PIN    (GPIOB->IDR)
#define TM_DIO_DDR    (GPIOB->DDR)
#define TM_DIO_BIT    (4)

#define TM_CLK_PORT   (GPIOB->ODR)
#define TM_CLK_PIN    (GPIOB->IDR)
#define TM_CLK_DDR    (GPIOB->DDR)
#define TM_CLK_BIT    (5)


/* DIO pin hardware layer */
#define TM_DIO_IN      TM_DIO_DDR  &= ~(1 << TM_DIO_BIT)
#define TM_DIO_OUT     TM_DIO_DDR  |=  (1 << TM_DIO_BIT)

#define TM_DIO_HIGH    TM_DIO_PORT |=  (1 << TM_DIO_BIT)
#define TM_DIO_LOW     TM_DIO_PORT &= ~(1 << TM_DIO_BIT)

#define TM_DIO_IS_HIGH ((TM_DIO_PIN & (1 << TM_DIO_BIT)) == (1 << TM_DIO_BIT))
#define TM_DIO_IS_LOW  (!TM_DIO_IS_HIGH)


/* CLK pin hardware layer */
#define TM_CLK_IN      TM_CLK_DDR  &= ~(1 << TM_CLK_BIT)
#define TM_CLK_OUT     TM_CLK_DDR  |=  (1 << TM_CLK_BIT)

#define TM_CLK_HIGH    TM_CLK_PORT |=  (1 << TM_CLK_BIT)
#define TM_CLK_LOW     TM_CLK_PORT &= ~(1 << TM_CLK_BIT)


/* Delay macro with 1us resolution */
#define DELAY_US(x)    hw_delay_us(x)


#endif /* TM1637_LL_H_ */
