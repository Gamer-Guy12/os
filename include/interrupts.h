#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_

#include <stdint.h>

void disable_interrupts(void);
void enable_interrupts(void);

// Handler takes in a pointer to the context which is arch specific
void register_int_handler(uint16_t interrupt, void handler(void *));

#endif
