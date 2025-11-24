#include "./paging.h"
#include "kernel/mem.h"
#include <stdint.h>

#define MB2_MASK (~0x1FFFFF)
#define GB_MASK (~0x3FFFFFFF)

void __map_pages(void *virt, void *phys, uint64_t page_count, uint32_t flags) {
  uintptr_t virt_bits = (uintptr_t)virt;
  uintptr_t phys_bits = (uintptr_t)phys;
  uint16_t page_flags = PAGE_ENTRY_PRESENT;

  if (flags & PM_RW) page_flags |= PAGE_ENTRY_RW;
  if (flags & PM_PINNED) page_flags |= PAGE_ENTRY_GLOBAL;
  if (flags & PM_WRITE_THROUGH) page_flags |= PAGE_ENTRY_WRITE_THROUGH;
}

void __unmap_pages(void *virt) {

}

