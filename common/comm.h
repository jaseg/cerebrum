
#ifndef __COMM_H__
#define __COMM_H__

#include <stdint.h>

void comm_handle(uint8_t c);

/*
 *comm callback function pointer type.
 *function signature:
 *uint8_t callback(uint8_t arglen, uint8_t* argbuf);
 *    payload_offst: number of bytes since the start of this payload
 *    arglen:        length of the argument string passed to the function
 *    argbuf:        buffer containing the argument string
 */
typedef void (*comm_callback)(uint16_t, uint16_t, uint8_t*);
extern const comm_callback comm_callbacks[];
extern const uint16_t callback_count;

#ifndef ARGBUF_SIZE
#define ARGBUF_SIZE 32
#endif

#endif//__COMM_H__

