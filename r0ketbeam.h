#include "cerebrum_module."
#ifndef __R0KETBEAM_H__
#define __R0KETBEAM_H__
#ifdef HAS_R0KETBEAM_SUPPORT

void r0ketbeam_loop(void);
void r0ketbeam_setup(void);

#else//HAS_R0KETBEAM_SUPPORT

void r0ketbeam_loop(){}
void r0ketbeam_setup(){}

#endif//HAS_R0KETBEAM_SUPPORT
#endif//__R0KETBEAM_H__
