
//CAUTION! This module uses Timer 0

#ifndef __PWM_H__
#define __PWM_H__

#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"

#ifdef HAS_PWM_SUPPORT

extern uint8_t pwm_cycle;
extern uint8_t pwm_val[];
#define PWM_COUNT   8

extern void pwm_output_setup(void);
extern void pwm_set_outputs(uint8_t data);
extern void pwm_unset_outputs(void);

#endif//HAS_PWM_SUPPORT

void pwm_setup(void);
void pwm_loop(void);

#endif//__PWM_H__
