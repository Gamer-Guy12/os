#ifndef MEMORY_H
#define MEMORY_H

#include <stdbool.h>
#include <stddef.h>

void *phys_alloc(void);
void *phys_free(void);

#endif
