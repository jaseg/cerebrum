
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
#ifdef PWM_ANIMATE
extern void pwm_animate(void);
#endif//PWM_ANIMATE

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb_value_t;

typedef union {
    rgb_value_t rgb_value;
    uint8_t components[3];
} rgb_color_t;

typedef struct {
    uint16_t h;
    uint8_t s;
    uint8_t v;
} hsv_value_t;

rgb_value_t hsv_to_rgb(hsv_value_t k);

#endif//HAS_PWM_SUPPORT

void pwm_setup(void);
void pwm_loop(void);

#endif//__PWM_H__
