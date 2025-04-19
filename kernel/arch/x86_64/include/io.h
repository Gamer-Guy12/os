#ifndef IO_H
#define IO_H

#include <stdint.h>

void outb(uint16_t port, uint8_t byte);
uint8_t inb(uint16_t port);
static inline void io_wait(void) { outb(0x80, 0); }

#endif
