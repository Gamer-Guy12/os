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

__attribute__((unused)) static struct page *pages =
    (struct page *)PAGE_STRUCT_ADDR;
#else
#error "Cannot define identity map offset"
#endif

enum page_flags { PAGE_MOVABLE = (1 << 0), PAGE_USER = (1 << 1) };

struct slab;

// Page data
struct page {
  // For Buddy Allocation
  page_ptr_t next;
  page_ptr_t prev;
  // 12 bytes available
  union {
    // For gheap
    struct {
      struct slab *slab;
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

#define AMETHOD_OFFSET 8
#define PAGE_FLAGS_OFFSET 16

#define GET_AMETHOD(flags) ((flags) & 0xFF)
#define GET_PAGE_FLAGS(flags) ((flags) & 0xFFFF)
#define GET_ZONE(flags) ((flags) & 0xFF)

// How it should be allocated
enum alloc_methods {
  AMETHOD_NOBLOCK = (0 << AMETHOD_OFFSET),
  AMETHOD_BLOCKING = (1 << AMETHOD_OFFSET),
};

// The flags a page should have for allocation
// Currently not implemented
enum page_alloc_flags {
  PF_USER = (1 << (PAGE_FLAGS_OFFSET)),
  PF_MOVABLE = (1 << (PAGE_FLAGS_OFFSET + 1))
};

enum alloc_flags {
  ALLOC_DMA = AMETHOD_NOBLOCK | ZONE_DMA,
  ALLOC_KERNEL = AMETHOD_BLOCKING | ZONE_HIGH,
  ALLOC_CRITICAL = AMETHOD_NOBLOCK | ZONE_HIGH,
  ALLOC_USER = AMETHOD_BLOCKING | ZONE_HIGH | PF_USER | PF_MOVABLE,
  ALLOC_COUNT = 4
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

void *alloc_pages(uint32_t order, uint32_t type);
void free_pages(void *addr, uint32_t order);

// Returns Max addr + 1
uintptr_t get_max_addr(void);

struct page *get_page(page_ptr_t ptr);

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
