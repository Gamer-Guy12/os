#ifndef MEM_H
#define MEM_H

#include <stddef.h>
#include <stdint.h>

void memset(void *ptr, uint8_t value, size_t num);
void memcpy(void *src, void *dest, size_t length);

#endif
