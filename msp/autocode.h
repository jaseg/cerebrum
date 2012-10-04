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
uint8_t callback_get_descriptor_auto(uint8_t alen, uint8_t* argbuf, uint8_t respbuflen, uint8_t* respbuf);

#endif//_AUTOCODE_H_
