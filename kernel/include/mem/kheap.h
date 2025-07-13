#ifndef KHEAP_H
#define KHEAP_H

#include <stddef.h>

typedef struct heap_entry_struct {
  size_t size;
  struct heap_entry_struct *next;
  struct heap_entry_struct *prev;
} heap_entry_t;

// Aligns the kernel heap to 8 bytes
void init_heap(void);

#endif
