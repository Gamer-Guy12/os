#ifndef _KERNEL_MEM_H_
#define _KERNEL_MEM_H_

#include "util.h"
#include <stdint.h>

#define PAGE_NULL 0xFFFFFFFF

// Lower bound: 3 * zone
// Upper bound: 3 * zone + 1
// Format: lower bound, upper bound, max order (capped at 10)
__attribute__((unused)) static void *zone_info[] = {
#ifdef _x86_64_
#define PAGE_SIZE 0x1000

#define ZONE_DMA 0
#define ZONE_LOW 1
#define ZONE_HIGH 2

    (void *)0,         (void *)(MB * 16 - 1), (void *)5,  // ZONE_DMA
    (void *)(MB * 16), (void *)(GB * 4 - 1),  (void *)10, // ZONE_LOW
    (void *)(4 * GB),  (void *)(MAX_64),      (void *)10  // ZONE_HIGH

#else
#error "Cannot find architecture, memory management cannot be done"
#endif
};

// Page data
struct page {
  uint32_t next;
  uint32_t prev;
};

// Holds everything to keep track of one set of buddies
struct buddy_data {
  uint32_t freelist;
  void *buddy_data;
};

// Everything for a zone
// A zone is a region of memory
struct zone {
  uint64_t max_order;
  void *start;
  void *end;
  struct buddy_data freelists[10];
};

enum alloc_flags {
  // Bits 0-2 are for the zone
  /// The page can be swapped out and moved throughout memory (not implemented
  /// yet)
  ALLOC_MOVABLE = (1 << 3),
};

// Math
#define ZONE_COUNT (sizeof(zone_info) / 3 / (sizeof(void *)))

void init_mem(void);

void *alloc_page(uint32_t order, uint32_t flags);
void free_page(void *addr, uint32_t order);

// Only for early on
void init_fmem(uintptr_t direct_offset);

void *fmem_palloc(void);
void fmem_pfree(void *addr);

void *fmem_valloc(void);
void fmem_vfree(void *addr);

void fmem_pfree_range(void *start, void *end);

#endif
