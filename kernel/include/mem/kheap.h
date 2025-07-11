#ifndef KHEAP_H
#define KHEAP_H

#include <stddef.h>

#define KERNEL_REGION_PTR_LOCATION                                             \
  ((vmm_kernel_region_t **)(260ull * 512ull * GB + (0xffffull << 48)))

typedef struct heap_entry_struct {
  size_t size;
  struct heap_entry_struct *next;
  struct heap_entry_struct *prev;
} heap_entry_t;

// Aligns the kernel heap to 8 bytes
void init_heap(void);

#endif
