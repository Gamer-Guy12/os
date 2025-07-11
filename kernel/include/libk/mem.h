#ifndef MEM_H
#define MEM_H

#include <stddef.h>
#include <stdint.h>

#define BYTES_PAST(ptr, bytes) (uint8_t *)ptr + bytes

void *memset(void *ptr, int value, size_t num);
void memcpy(void *src, void *dest, size_t length);

#define KB 0x400
#define MB 0x100000
#define GB 0x40000000

#endif
