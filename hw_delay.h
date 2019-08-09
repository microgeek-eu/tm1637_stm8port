/*
 * hw_delay.h
 *
 *  Created on: 7 sie 2019
 *      Author: Zbigniew Gibek
 */

#ifndef HW_DELAY_H_
#define HW_DELAY_H_

/* Values are valid for 16MHz internal clock! */
#define TIM4_MS_PERIOD     124
#define TIM4_US_PERIOD      15


void hw_delay_init(void);
void hw_delay_us(uint16_t us);
void hw_delay_ms(uint16_t ms);

#endif /* HW_DELAY_H_ */