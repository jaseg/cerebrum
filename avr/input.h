
#ifndef __INPUTS_H__
#define __INPUTS_H__
#include <avr/io.h>
#include "config.h"
#include "util.h"
#include "uart.h"

void input_setup(void);
void input_loop(void);

#ifdef HAS_INPUT_SUPPORT

extern uint8_t debounce_timeouts[INPUT_COUNT];
extern uint8_t switch_states[INPUT_COUNT];
void input_scan_inputs(void);
void input_callback(uint8_t input, uint8_t state);

#endif//HAS_INPUT_SUPPORT
#endif//__INPUTS_H__
