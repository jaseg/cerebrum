#ifndef __7SEG_H__
#define __7SEG_H__

#include <avr/io.h>

void 7seg_setup(void);
void 7seg_loop(void);

uint8_t 7seg_buf[4];

#endif//__7SEG_H__
