#ifndef MEMORY_H
#define MEMORY_H

#include <stdbool.h>
#include <stddef.h>

/// Allocate a physical page
void *phys_alloc(void);

/// Free a physical page
void phys_free(void *addr);

#endif
