#ifndef _KERNEL_MEM_H_
#define _KERNEL_MEM_H_

#include "lib/spinlock.h"
#include "limine.h"
#include "util.h"
#include <stdint.h>

#define PAGE_NULL 0xFFFFFFFFFFFFFFFF

#ifdef _x86_64_
#define IDENTITY_MAP_OFFSET 0xFFFF800000000000
#define PAGE_STRUCT_ADDR 0xFFFFC00000000000
#define BUDDY_DATA_ADDR 0xFFFFC08000000000
#define KERNEL_ADDR 0xFFFFFFFF80000000

__attribute__((unused)) static struct page *pages =
    (struct page *)PAGE_STRUCT_ADDR;
#else
#error "Cannot define identity map offset"
#endif

// Lower bound: 3 * zone
// Upper bound: 3 * zone + 1
// Format: lower bound, upper bound, max order (capped at 10)
__attribute__((unused)) static INIT_DATA void *zone_info[] = {
#ifdef _x86_64_
#define PAGE_SIZE 0x1000ull

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
  uint64_t next;
  uint64_t prev;
};

// Holds everything to keep track of one set of buddies
struct buddy_data {
  uint64_t freelist;
  void *buddy_data;
};

// Everything for a zone
// A zone is a region of memory
struct zone {
  struct buddy_data freelists[10];
  uint64_t max_order;
  void *start;
  void *end;
  spinlock_t lock;
};

enum alloc_flags {
  // Bits 0-2 are for the zone
  /// The page can be swapped out and moved throughout memory (not implemented
  /// yet)
  ALLOC_MOVABLE = (1 << 3),
  ALLOC_ZONE_ANY = (1 << 4)
};

// Math
#define ZONE_COUNT (sizeof(zone_info) / 3 / (sizeof(void *)))

void init_mem(void);

// These are the series of things that happen in memory initialization
// Any of these that are architecture dependent can do literally anything but
// its best to follow what these set out
//
// There are also phases in between these that are statically declared
void init_fmem(uintptr_t direct_offset);
void init_paging(
    uint64_t map_entry_count,
    struct limine_memmap_entry **map_entries); // Architecture Dependent
void init_buddy(void);

void *alloc_page(uint32_t order, uint32_t flags);
void free_page(void *addr, uint32_t order);

// Returns Max addr + 1
uintptr_t get_max_addr(void);

// Only for early on
void *fmem_palloc(void);
void fmem_pfree(void *addr);

void *fmem_valloc(void);
void fmem_vfree(void *addr);

void fmem_pfree_range(void *start, void *end);

#endif
