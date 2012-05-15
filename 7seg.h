#ifndef __7SEG_H__
#define __7SEG_H__
#ifdef HAS_7SEG_SUPPORT

#include <avr/io.h>

void 7seg_setup(void);
void 7seg_loop(void);

uint8_t 7seg_buf[1] = {char[4]};

#endif//HAS_7SEG_SUPPORT
#endif//__7SEG_H__
