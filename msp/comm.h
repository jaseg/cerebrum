
#ifndef __COMM_H__
#define __COMM_H__

void comm_loop(void);
void putc_escaped(char c);

/*
 *comm callback function pointer type.
 *function signature:
 *uint8_t callback(uint8_t arglen, uint8_t* argbuf, uint8_t respbuflen, uint8_t* respbuf);
 *    arglen: length of the argument string passed to the function
 *    argbuf: buffer containing the argument string
 *    respbuflen: length of the response buffer (maximum response length)
 *    respbuf: response buffer (write your return value *here*)
 *
 *    return value: amount of data written to respbuf (in bytes)
 */
typedef void (*comm_callback)(uint16_t, uint8_t*);
extern comm_callback comm_callbacks[];

#ifndef NUM_CALLBACKS
#define NUM_CALLBACKS 0
#endif//NUM_CALLBACKS

#ifndef ARGBUF_SIZE
#define ARGBUF_SIZE 32
#endif//ARGBUF_SIZE

#endif//__COMM_H__

