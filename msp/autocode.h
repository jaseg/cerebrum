/*
 * Header file for autocode.h
 * --------------------------
 *  This file actually is written by hand.
 */
#ifndef _AUTOCODE_H_
#define _AUTOCODE_H_
typedef unsigned char uint8_t;
typedef unsigned int uint16_t;

void init_auto(void);
void loop_auto(void);
void callback_get_descriptor_auto(uint16_t alen, uint8_t* argbuf);

#endif//_AUTOCODE_H_
