/// @file This file is for kernel io (kio)

#ifndef KIO_H
#define KIO_H

#include <stdint.h>

void kio_puts(char *c);
void kio_printf(const char *format, ...);
void kio_clear(void);

#endif
