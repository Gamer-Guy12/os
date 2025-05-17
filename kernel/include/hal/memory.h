#ifndef MEMORY_H
#define MEMORY_H

#include <stdbool.h>
#include <stddef.h>

void *phys_alloc(void);
void *phys_alloc_at(void *addr);

/// It should return NULL
void *phys_free(void *addr);

void *virt_map_at(void *phys_addr, void *virt_addr);

/// Maps into kernel space
void *virt_kernel_map(void *addr);
/// Maps into user space
void *virt_user_map(void *addr);

/// Should return NULL
void *virt_unmap(void *addr);

#endif
