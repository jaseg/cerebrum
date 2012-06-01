#ifndef __7SEG_H__
#define __7SEG_H__

#include <avr/io.h>
#include "config.h"

void l7seg_setup(void);
void l7seg_loop(void);

#ifdef HAS_7SEG_SUPPORT

extern uint8_t* l7seg_buf;

#endif//HAS_7SEG_SUPPORT
#endif//__7SEG_H__
