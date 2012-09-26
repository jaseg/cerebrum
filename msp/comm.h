
#ifndef __COMM_H__
#define __COMM_H__

void comm_loop(void);
void putc_escaped(char c);

//callbacks
extern uint8_t comm_get_reg(uint16_t address);
extern void comm_set_reg(uint16_t address, uint8_t val);

#endif//__COMM_H__

