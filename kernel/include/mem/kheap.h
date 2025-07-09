#ifndef KHEAP_H
#define KHEAP_H

#include <libk/mem.h>
#include <mem/memory.h>
#include <stddef.h>
#include <stdint.h>

#define KERNEL_REGION_PTR_LOCATION                                             \
  ((vmm_kernel_region_t **)(260ull * 512ull * GB + (0xffffull << 48)))

typedef struct heap_entry_struct {
  size_t size;
  union {
    /// Points to a higher address, the start of the heap contains the
    /// first entry and is searched upwards from there
    struct heap_entry_struct *next;
    struct {
      uint64_t free : 1;
      uint64_t ignored : 2;
      uint64_t dont_use : 61;
    };
  };
} heap_entry_t;

// Aligns the kernel heap to 8 bytes
void init_heap(void);

#endif
