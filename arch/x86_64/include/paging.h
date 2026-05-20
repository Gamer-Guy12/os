#ifndef _x86_64_PAGING_H_
#define _x86_64_PAGING_H_

#include <stdint.h>

#define ADDR_MASK 0x0007FFFFFFFFF000ull

enum page_levels {
  PAGE_LEVEL_PAGE = 0,
  PAGE_LEVEL_PT = 1,
  PAGE_LEVEL_PDT = 2,
  PAGE_LEVEL_PDPT = 3,
  PAGE_LEVEL_PML4 = 4,
};

enum page_entry_flags {
  PAGE_ENTRY_PRESENT = 1 << 0,
  PAGE_ENTRY_RW = 1 << 1,
  PAGE_ENTRY_USER = 1 << 2,
  PAGE_ENTRY_WRITE_THROUGH = 1 << 3,
  PAGE_ENTRY_WT = 1 << 3,
  PAGE_ENTRY_CACHE_DISABLED = 1 << 4,
  PAGE_ENTRY_UC = 1 << 4,
  PAGE_ENTRY_ACCESSED = 1 << 5,
  PAGE_ENTRY_DIRTY = 1 << 6,
  // Should the MMU use the PAT
  PAGE_ENTRY_ATTRIBUTE_TABLE = 1 << 7,
  PAGE_ENTRY_PAT = 1 << 7,
  PAGE_ENTRY_HUGE = 1 << 7,
  PAGE_ENTRY_GLOBAL = 1 << 8,
  // Same thing as PAGE_ENTRY_PAT except for huge pages
  PAGE_ENTRY_HUGE_PAT = 1 << 12
};

struct page_entry {
  union {
    uint64_t addr;
    uint64_t entry;
    struct {
      uint64_t flags : 12;
      uint64_t addr_chunk : 39;
      uint64_t reserved : 1;
      uint64_t ignored : 7;
      uint64_t mpk : 4;
      uint64_t nx : 1;
    };
  };
};

#endif
