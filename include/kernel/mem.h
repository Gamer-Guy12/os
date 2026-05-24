#ifndef _KERNEL_MEM_H_
#define _KERNEL_MEM_H_

#include "limine.h"
#include <stddef.h>
#include <stdint.h>

#ifdef _x86_64_

#define PAGE_SIZE 0x1000
#define MAX_ORDER 10
#define IDENTITY_OFFSET 0xFFFF800000000000ull
#define PAGE_STRUCT_OFFSET 0xFFFFC00000000000ull
#define BUDDY_OFFSET 0xFFFFC08000000000ull
#define KERNEL_OFFSET 0xFFFFFFFF80000000ull

// Page pointers are relative in a zone
typedef uint64_t pageptr_t;

#else
#error "Could not define memory constants"
#endif

__attribute__((unused)) static struct page *pages = (struct page *)PAGE_STRUCT_OFFSET;

// Virtual to physical
#define VTP(addr) (void *)((uintptr_t)(addr) - IDENTITY_OFFSET)
// Physical to virtual
#define PTV(addr) (void *)((uintptr_t)(addr) + IDENTITY_OFFSET)

struct page {
  struct page *next;
  struct page *prev;
};

// One layer in the buddy table
struct buddy_layer {
  struct page *freelist;
  void *data;
};

enum zone_type {
  ZONE_DMA,
  ZONE_NORMAL,
  ZONE_COUNT
};

// Zone of length zero will be sized during init
struct zone {
  struct buddy_layer buddy[MAX_ORDER];
  void *base;
  uintptr_t length;
  int type;
};

enum map_flags {
  // Read/Write
  MAP_RW = 1 << 0,
  // Write through
  MAP_WT = 1 << 1,
  // Uncacheable
  MAP_UC = 1 << 2,
  // Same as x86_64 global
  MAP_PINNED = 1 << 3,
  // Not Executable
  MAP_NX = 1 << 4,
  // Accessable by users
  MAP_USER = 1 << 5,
};

// Arch specific
struct zone *__get_zone(int zone);
// get_virt_page returns an identity mapped virtual page
// return value will be null if get_virt_page returns null
struct page *__map_phys_pages(void *vaddr, void *paddr, int flags, void *(get_phys_page)(void), size_t count);
struct page *__map_page(void *vaddr, int flags, void *(get_phys_page)(void));

// Internal
struct page *__get_page_struct(pageptr_t ptr, int zone);
pageptr_t __get_page_pointer(struct page *page, int zone);
struct page *__paddr_page_struct(void *addr);

void init_mem(void);
// Reads memory map and moves it to fmem
void read_memmap(void);
// Copies old page tables and creates new ones as needed
void __init_page_tables(size_t entry_count, struct limine_memmap_entry **entries);
// Creates zones and buddy data
void calculate_mem_sizes(void);

#endif
