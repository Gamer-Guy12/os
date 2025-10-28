#ifndef _x86_64_MEML_PAGING_H_
#define _x86_64_MEML_PAGING_H_

#include <stdint.h>

enum page_flags {
  PAGE_PRESENT = 1 << 0,
  PAGE_RW = 1 << 1,
  PAGE_USER = 1 << 2,
  PAGE_WRITE_THROUGH = 1 << 3,
  PAGE_WT = 1 << 3,
  PAGE_CACHE_DISABLED = 1 << 4,
  PAGE_UC = 1 << 4,
  PAGE_ACCESSED = 1 << 5,
  PAGE_DIRTY = 1 << 6,
  // Should the MMU use the PAT
  PAGE_ATTRIBUTE_TABLE = 1 << 7,
  PAGE_PAT = 1 << 7,
  PAGE_HUGE = 1 << 7,
  PAGE_GLOBAL = 1 << 8,
  PAGE_HUGE_PAT = 1 << 12,
};

struct paging_entry {
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

