#ifndef _KERNEL_MEM_H_
#define _KERNEL_MEM_H_

#include "lib/spinlock.h"
#include "limine.h"
#include "util.h"
#include <stddef.h>
#include <stdint.h>

#define PAGE_NULL 0xFFFFFFFFFFFFFFFF
#define ZONE_NULL 0xFFFFFFFF

typedef uint64_t page_ptr_t;

#ifdef _x86_64_
#define IDENTITY_MAP_OFFSET 0xFFFF800000000000
#define PAGE_STRUCT_ADDR 0xFFFFC00000000000
#define BUDDY_DATA_ADDR 0xFFFFC08000000000
#define KERNEL_ADDR 0xFFFFFFFF80000000
#define PAGE_SIZE 0x1000ull
#define MAX_ORDER 10
#else
#error                                                                         \
    "Cannot determine architecture dependent memory information. Please define everything that is defined in the x86_64 architecture."
#endif

__attribute__((unused)) static struct page *pages =
    (struct page *)PAGE_STRUCT_ADDR;

// Page data
struct page {
  // For Buddy Allocation
  page_ptr_t next;
  page_ptr_t prev;
  // 12 bytes available
  union {
    // For gheap
    struct {
      struct gheap_slab *slab;
    };
    uint8_t reserved[12];
  } __attribute__((packed));
  uint32_t flags;
};

SASSERT(sizeof(struct page) == 32, "Incorrect size of page struct");

// Holds everything to keep track of one set of buddies
struct buddy_data {
  page_ptr_t freelist;
  void *buddy_data;
};

// Everything for a zone
// A zone is a region of memory
struct zone {
  struct buddy_data freelists[MAX_ORDER];
  uintptr_t start;
  uintptr_t end;
  spinlock_t lock;
};

enum zones {
  ZONE_DMA,
  ZONE_LOW,
  ZONE_HIGH,
  ZONE_COUNT,
};

#define ZONE_ANY ZONE_HIGH
#define ZONE_NULL 0xFFFFFFFF

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
void init_gheap(void);

void *__alloc_pages(uint32_t order, uint32_t zone);
void __free_pages(void *addr, uint32_t order);

#define GET_ZONE(flags) ((flags) & 0xFF)
#define GET_METHOD(flags) ((flags) & 0xFF00)
#define GET_PAGE_FLAGS(flags) ((flags) & 0xFFFF0000)

void *alloc_pages(uint32_t order, uint32_t flags);
void free_pages(void *addr, uint32_t order);

enum pm_flags {
  PM_RW = (1 << 0),
  PM_EXEC = (1 << 1),
  // Same as x86_64 global
  PM_PINNED = (1 << 2),
  PM_WRITE_THROUGH = (1 << 3)
};

// Aligned mapping is always preferred
//
// Allocates all intermediate pages
// Does not clean tlb
void __map_pages(void *virt, void *phys, uint64_t page_count, uint32_t flags);
// Frees all unnecessary intermediate pages
// Does not clean tlb
void __unmap_pages(void *virt);

// Returns Max addr + 1
uintptr_t get_max_addr(void);

struct page *get_page(page_ptr_t ptr);
struct page *addr_page(void *addr);

/// Converts an architecture independent zone to a dependent one (all other zone
/// functions require architecture dependent zones)
uint32_t get_zone(uint32_t zone);
uint32_t get_zone_fallback(uint32_t zone);
void get_zone_info(uint32_t zone, uintptr_t *start, uintptr_t *end);

// Only for early on
void *fmem_palloc(void);
void fmem_pfree(void *addr);

void *fmem_valloc(void);
void fmem_vfree(void *addr);

void fmem_pfree_range(void *start, void *end);

#endif
