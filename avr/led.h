
#ifndef __LED_H__
#define __LED_H__

#include <avr/io.h>
#include "config.h"

#ifdef HAS_LED_SUPPORT

extern uint8_t* frameBuffer;
extern uint8_t* secondFrameBuffer;

extern void led_output_stuff1(uint8_t data, uint8_t dir);

void swapBuffers(void);
void setLED(int num, int val);

#endif//HAS_LED_SUPPORT

//CAUTION! This loop function takes more than 8ms due to delays.
void led_loop(void);
void led_setup(void);

#endif//__LED_H__
