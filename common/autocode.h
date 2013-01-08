/*
 * Header file for autocode.h
 * --------------------------
 *  This file actually is written by hand.
 */
#ifndef _AUTOCODE_H_
#define _AUTOCODE_H_

#include <stdint.h>

#include "comm.h"

void init_auto(void);
void loop_auto(void);
void callback_get_descriptor_auto(const comm_callback_descriptor* cb, void* argbuf_end);

#endif//_AUTOCODE_H_
