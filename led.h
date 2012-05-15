
#ifndef __LED_H__
#define __LED_H__

#include <avr/io.h>

#ifdef HAS_LED_SUPPORT

//this scary construct is in place to make the compiler happy. if you know a better way, feel free to improve.
uint8_t _frameBuffer[] = {0,0,0,0};
uint8_t _secondFrameBuffer[] = {0,0,0,0};
uint8_t* frameBuffer = _frameBuffer;
uint8_t* secondFrameBuffer = _secondFrameBuffer;

void swapBuffers(void);
void setLED(int num, int val);

//CAUTION! This loop function takes more than 8ms due to delays.
void led_loop(void);

#else//HAS_LED_SUPPORT

void led_loop(void){}

#endif//HAS_LED_SUPPORT

//The DDRs of the led matrix outputs are set in the mux loop.
void led_setup(void){}

#endif//__LED_H__
