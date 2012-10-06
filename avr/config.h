#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "avr/io.h"

#define HAS_PWM_SUPPORT 1

void config_setup(void);
void config_loop(void);

#endif//__CONFIG_H__
