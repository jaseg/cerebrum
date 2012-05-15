
#ifndef __INPUTS_H__
#define __INPUTS_H__
#ifdef HAS_INPUT_SUPPORT
#include <avr/io.h>

uint8_t debounce_timeouts[INPUT_COUNT];
uint8_t switch_states[INPUT_COUNT];

void input_setup(void);
void input_loop(void);

#else//HAS_INPUT_SUPPORT

void input_setup(){}
void input_loop(){}

#endif//HAS_INPUT_SUPPORT
#endif//__INPUTS_H__
