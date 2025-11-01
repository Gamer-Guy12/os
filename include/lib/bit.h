#ifndef _LIB_BIT_H_
#define _LIB_BIT_H_

#include <stdbool.h>
#include <stddef.h>

void flip_bit_in_ptr(void *ptr, size_t bit);
bool check_bit_in_ptr(void *ptr, size_t bit);

#endif

