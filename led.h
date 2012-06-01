
#ifndef __LED_H__
#define __LED_H__

#include <avr/io.h>
#include "config.h"

#ifdef HAS_LED_SUPPORT

extern uint8_t* frameBuffer;
extern uint8_t* secondFrameBuffer;

void swapBuffers(void);
void setLED(int num, int val);

//CAUTION! This loop function takes more than 8ms due to delays.
void led_loop(void);

#else//HAS_LED_SUPPORT

void led_loop(){}

#endif//HAS_LED_SUPPORT

void led_setup(void);

#endif//__LED_H__
