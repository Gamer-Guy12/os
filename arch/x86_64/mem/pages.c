#include "./paging.h"
#include "kernel/mem.h"
#include <stdint.h>

#define PML4_INDEX(addr) (((uintptr_t)(addr) >> 39) & 0x1ff)
#define PDPT_INDEX(addr) (((uintptr_t)(addr) >> 30) & 0x1ff)
#define PDT_INDEX(addr) (((uintptr_t)(addr) >> 21) & 0x1ff)
#define PT_INDEX(addr) (((uintptr_t)(addr) >> 12) & 0x1ff)
#define ADDR_MASK 0x0007FFFFFFFFF000ull

static uintptr_t phys_to_virt(void *phys) {
  return (uintptr_t)phys + IDENTITY_MAP_OFFSET;
}

// Size of 0 means 4kb page
// Size of 1 means 2mb page
// Size of 2 means 1gb page
//
// Returns whether it actually mapped the page or not
static INIT bool __map_page(uintptr_t phys_addr, void *map_addr, uint16_t flags,
                            bool nx, uint8_t size, void *cr3) {
  struct paging_entry *pml4 = (void *)((uintptr_t)cr3 + IDENTITY_MAP_OFFSET);
  const size_t pml4_index = PML4_INDEX(map_addr);

  struct paging_entry *pdpt = NULL;
  const size_t pdpt_index = PDPT_INDEX(map_addr);

  // PDPTs are gaurenteed to be there
  uintptr_t pdpt_phys_addr = (uintptr_t)alloc_pages(0, ZONE_HIGH);
  pdpt = (void *)phys_to_virt((void *)pdpt_phys_addr);
  pml4[pml4_index].addr = pdpt_phys_addr;
  pml4[pml4_index].flags = flags;
  pml4[pml4_index].nx = nx;

  if (size == 2) {
    goto map_gb_page;
  }

  struct paging_entry *pdt = NULL;
  const size_t pdt_index = PDT_INDEX(map_addr);

  if (pdpt[pdpt_index].flags & PAGE_ENTRY_PRESENT) {
    const size_t phys_addr = pdpt[pdpt_index].addr & ADDR_MASK;
    const size_t virt_addr = phys_addr + IDENTITY_MAP_OFFSET;
    pdt = (void *)virt_addr;
  } else {
    uintptr_t phys_addr = (uintptr_t)alloc_pages(0, ZONE_HIGH);
    pdt = (void *)(phys_to_virt((void *)phys_addr));
    pdpt[pdpt_index].addr = phys_addr;
    pdpt[pdpt_index].flags = flags;
    pdpt[pdpt_index].nx = nx;
  }

  if (size == 1) {
    goto map_mb_page;
  }

  struct paging_entry *pt = NULL;
  const size_t pt_index = PT_INDEX(map_addr);

  if (pdt[pdt_index].flags & PAGE_ENTRY_PRESENT) {
    const size_t phys_addr = pdt[pdt_index].addr & ADDR_MASK;
    const size_t virt_addr = phys_addr + IDENTITY_MAP_OFFSET;
    pt = (void *)virt_addr;
  } else {
    uintptr_t phys_addr = (uintptr_t)alloc_pages(0, ZONE_HIGH);
    pt = (void *)(phys_to_virt((void *)phys_addr));
    pdt[pdt_index].addr = phys_addr;
    pdt[pdt_index].flags = flags;
    pdt[pdt_index].nx = nx;
  }

  if (!(pt[pt_index].flags & PAGE_ENTRY_PRESENT)) {
    pt[pt_index].addr = phys_addr;
    pt[pt_index].flags = flags;
    pt[pt_index].nx = nx;
    return true;
  }
  return false;

map_gb_page:
  if (!(pdpt[pdpt_index].flags & PAGE_ENTRY_PRESENT)) {
    flags |= PAGE_ENTRY_HUGE;
    pdpt[pdpt_index].addr = phys_addr;
    pdpt[pdpt_index].flags = flags;
    pdpt[pdpt_index].nx = nx;
    return true;
  }
  return false;

map_mb_page:
  if (!(pdt[pdt_index].flags & PAGE_ENTRY_PRESENT)) {
    flags |= PAGE_ENTRY_HUGE;
    pdt[pdt_index].addr = phys_addr;
    pdt[pdt_index].flags = flags;
    pdt[pdt_index].nx = nx;
    return true;
  }

  return false;
}

void *map_phys(void *phys, uint32_t flags) {
  void *virt = (void *)((uintptr_t)phys + IDENTITY_MAP_OFFSET);
  void *cr3;
  __asm__ volatile("mov %%cr3, %0" : "=r"(cr3));

  uint16_t map_flags = PAGE_ENTRY_PRESENT | PAGE_ENTRY_GLOBAL;

  if (flags & PM_RW)
    map_flags |= PAGE_ENTRY_RW;
  if (flags & PM_WRITE_THROUGH)
    map_flags |= PAGE_ENTRY_WRITE_THROUGH;

  __map_page((uintptr_t)phys, virt, map_flags, !(flags & PM_EXEC), 0, cr3);

  return virt;
}
