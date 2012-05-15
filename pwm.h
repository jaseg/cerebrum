
//CAUTION! This module uses Timer 0

#ifndef __PWM_H__
#define __PWM_H__
#ifdef HAS_PWM_SUPPORT
#include <avr/io.h>

uint8_t pwm_cycle = 0;
uint8_t pwm_val[PWM_COUNT];

#endif//HAS_PWM_SUPPORT
#endif//__PWM_H__
