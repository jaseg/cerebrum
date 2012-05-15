
//CAUTION! This module uses Timer 0

#ifndef __PWM_H__
#define __PWM_H__

#include <avr/io.h>

#ifdef HAS_PWM_SUPPORT

uint8_t pwm_cycle = 0;
uint8_t pwm_val[PWM_COUNT];

void pwm_setup(void);

#else//HAS_PWM_SUPPORT

void pwm_setup(){}

#endif//HAS_PWM_SUPPORT

//The PWM has no loop function. It runs entirely out of the ISR.
void pwm_loop(){}

#endif//__PWM_H__
