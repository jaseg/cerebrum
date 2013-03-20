
#ifndef __COMM_H__
#define __COMM_H__

#include <stdint.h>

/*
 *comm callback function pointer type.
 *function signature:
 *uint8_t callback(uint8_t arglen, uint8_t* argbuf);
 *    payload_offst: number of bytes since the start of this payload
 *    arglen:        length of the argument string passed to the function
 *    argbuf:        buffer containing the argument string
 */
typedef struct comm_callback_descriptor comm_callback_descriptor;
typedef void (*comm_callback)(const comm_callback_descriptor* cb, void* argbuf_end);
struct comm_callback_descriptor {
	comm_callback callback;
	void* argbuf;
	uint16_t argbuf_len;
};
typedef struct {
	comm_callback_descriptor const * descriptor;
	void* argbuf_end;
} callback_stack_t;

extern const comm_callback_descriptor comm_callbacks[];
extern const uint16_t callback_count;

extern volatile callback_stack_t next_callback;
extern const volatile uint8_t global_argbuf[];

#ifndef ARGBUF_SIZE
//needs to be at least 4 bytes to accomodate args_t in comm_handle(uint8_t)
#ifndef __TEST__
#define ARGBUF_SIZE 32
#else
#define ARGBUF_SIZE 257
#endif
#endif

#define ADDRESS_DISCOVERY 0xFFFF

void comm_loop(void);

#endif//__COMM_H__

