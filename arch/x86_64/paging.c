#include "paging.h"
#include "kernel/fmem.h"
#include "kernel/kprintf.h"
#include "kernel/mem.h"
#include "limine.h"
#include "util.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define ROUND_UP(num, to) ((((num) + (to) - 1) / (to)) * (to))

struct page *__do_phys_map(void *vaddr, void *paddr, int flags,
                           void *(get_phys_page)(void), int level,
                           struct page_entry *pml4, int nx) {
  int higher_global = PAGE_ENTRY_GLOBAL * ((uintptr_t)vaddr >= IDENTITY_OFFSET);

  if (level > 0)
    flags |= PAGE_ENTRY_HUGE;
  flags |= higher_global;

  const size_t pt_index = ((uintptr_t)vaddr >> 12) & 0x1FF;
  const size_t pdt_index = ((uintptr_t)vaddr >> 21) & 0x1FF;
  const size_t pdpt_index = ((uintptr_t)vaddr >> 30) & 0x1FF;
  const size_t pml4_index = ((uintptr_t)vaddr >> 39) & 0x1FF;

  struct page_entry *entry = NULL;
  struct page_entry *pdpt = NULL;
  struct page_entry *pdt = NULL;
  struct page_entry *pt = NULL;

  // Put PDPT into PML4
  // Skip if PDPT is already in PML4
  if (!(pml4[pml4_index].flags & PAGE_ENTRY_PRESENT)) {
    pml4[pml4_index].addr = (uintptr_t)get_phys_page();
    pml4[pml4_index].flags = PAGE_ENTRY_PRESENT | PAGE_ENTRY_RW | higher_global;
    pml4[pml4_index].nx = 0;
  }

  pdpt = PTV(pml4[pml4_index].addr & ADDR_MASK);

  // PDPT is in PML4
  if (level > PAGE_LEVEL_PT) {
    entry = &pdpt[pdpt_index];
    goto map_page;
  }

  // Put PDT into PDPT
  // Skip if PDT is already in PDPT
  if (!(pdpt[pdpt_index].flags & PAGE_ENTRY_PRESENT)) {
    pdpt[pdpt_index].addr = (uintptr_t)get_phys_page();
    pdpt[pdpt_index].flags = PAGE_ENTRY_PRESENT | PAGE_ENTRY_RW | higher_global;
    pdpt[pdpt_index].nx = 0;
  }

  pdt = PTV(pdpt[pdpt_index].addr & ADDR_MASK);

  // PDT is in PDPT
  if (level > PAGE_LEVEL_PAGE) {
    entry = &pdt[pdt_index];
    goto map_page;
  }

  // Put PT into PDT
  // Skip if PT is already in PDT
  if (!(pdt[pdt_index].flags & PAGE_ENTRY_PRESENT)) {
    pdt[pdt_index].addr = (uintptr_t)get_phys_page();
    pdt[pdt_index].flags = PAGE_ENTRY_PRESENT | PAGE_ENTRY_RW | higher_global;
    pdt[pdt_index].nx = 0;
  }

  pt = PTV(pdt[pdt_index].addr & ADDR_MASK);
  entry = &pt[pt_index];

map_page:
  entry->addr = (uintptr_t)paddr;
  entry->flags = flags;
  entry->nx = nx;

  return __paddr_page_struct(paddr);
}

struct page *__map_phys_pages(void *vaddr, void *paddr, int flags,
                              void *(get_phys_page)(void), size_t count) {
  uint64_t cr3 = 0;
  __asm__ volatile("mov %%cr3, %0" : "=r"(cr3));
  struct page_entry *pml4 = (struct page_entry *)(cr3 + IDENTITY_OFFSET);

  int nx = 0;
  if (flags & MAP_NX)
    nx = 1;

  int page_flags = PAGE_ENTRY_PRESENT;
  if (flags & MAP_RW)
    page_flags |= PAGE_ENTRY_RW;
  if (flags & MAP_WT)
    page_flags |= PAGE_ENTRY_WRITE_THROUGH;
  if (flags & MAP_UC)
    page_flags |= PAGE_ENTRY_UC;
  if (flags & MAP_PINNED)
    page_flags |= PAGE_ENTRY_GLOBAL;
  if (flags & MAP_USER)
    page_flags |= PAGE_ENTRY_USER;

  // Fix to have failure methods and rollback and huge pages
  for (size_t i = 0; i < count; i++) {
    if (!__do_phys_map(vaddr, paddr, page_flags, get_phys_page, 0, pml4, nx)) {
      return NULL;
    }
  }

  return __paddr_page_struct(paddr);
}

struct page *__map_page(void *vaddr, int flags, void *(get_phys_page)(void)) {
  void *addr = get_phys_page();
  if (addr == NULL) {
    return NULL;
  }
  return __map_phys_pages(vaddr, addr, flags, get_phys_page, 1);
}

// Maps a physical address to virtual and returns virtual address
//
// Level:
// - PAGE_LEVEL_PAGE - 4Kb
// - PAGE_LEVEL_PT - 2 Mb
// - PAGE_LEVEL_PDT - 1 Gb
// PAGE_ENTRY_HUGE not needed
// PAGE_ENTRY_GLOBAL by default (can't be changed)
//
// Assumes nx
static void *__imap_page(void *phys, uint16_t flags, int level,
                         struct page_entry *pml4) {
  __do_phys_map(PTV(phys), phys, flags | PAGE_ENTRY_GLOBAL, _fmem_phys, level,
                pml4, 1);

  return PTV(phys);
}

// Inputs
// Level of the inputted entry (if it is part of the pml4 it should be
// PAGE_LEVEL_PML4) For example when inputing a pointer to the pml4 level should
// be PAGE_LEVEL_PML4 Pointer to (virtual) page that contains the table that
// should be copied Pointer to (virtual) page that contains the table that
// should be copied to
static void __clone_mappings(struct page_entry *src, struct page_entry *dst,
                             const int level, int start) {
  // If the table contains raw data (e.g. either level = PAGE_LEVEL_PT or
  // PAGE_ENTRY_HUGE) then the data should be copied elsewise the table should
  // be remade
  for (int i = start; i < 512; i++) {
    if (level == PAGE_LEVEL_PT || src[i].flags & PAGE_ENTRY_HUGE) {
      dst[i] = src[i];
    } else {
      // Create new table
      dst[i].addr = (uintptr_t)VTP(_fmem_alloc());
      dst[i].nx = src[i].nx;
      dst[i].flags = src[i].flags;

      __clone_mappings(PTV(src[i].addr & ADDR_MASK),
                       PTV(dst[i].addr & ADDR_MASK), level - 1, 0);
    }
  }
}

static void __create_top_pages(struct page_entry *pml4) {
  for (int i = 256; i < 512; i++) {
    void *entry_addr = _fmem_alloc();
    if (entry_addr == NULL) {
      _kprintf("Failed to alloc page for pdpt: 0x%x\n", i);
      panic();
    }
    pml4[i].addr = (uintptr_t)VTP(entry_addr);
    pml4[i].flags = PAGE_ENTRY_PRESENT | PAGE_ENTRY_RW | PAGE_ENTRY_GLOBAL;
    if (i == 511)
      pml4[i].nx = 0;
    else
      pml4[i].nx = 1;
  }
}

// Get old cr3, recursively clone tables
static void __clone_kernel_mappings(const uintptr_t new_cr3) {
  uintptr_t old_cr3;
  __asm__ volatile("mov %%cr3, %0" : "=r"(old_cr3));

  struct page_entry *old_pml4 = PTV(old_cr3);
  struct page_entry *new_pml4 = PTV(new_cr3);
  __create_top_pages(new_pml4);

  // The pointer is pointing to a page of the PDPT
  // Only copy the last 2 gigabytes
  __clone_mappings(PTV(old_pml4[511].addr & ADDR_MASK),
                   PTV(new_pml4[511].addr & ADDR_MASK), PAGE_LEVEL_PDPT, 510);
}

// Checks how many bits are unset at the bottom
// Depending on the amount it tells the alignment
// Then checks if there are enough pages left
static int __calculate_level(size_t base, size_t page_count) {
  // Checking if the bottom 30 bits are set
  if ((base & 0x3FFFFFFF) == 0 && page_count >= 262144) {
    return PAGE_LEVEL_PDT;
    // Checks if bottom 21 bits are set
  } else if ((base & 0x1FFFFF) == 0 && page_count >= 512) {
    return PAGE_LEVEL_PT;
  }

  return PAGE_LEVEL_PAGE;
}

// Returns pages mapped
static size_t __do_identity_map(uintptr_t base, size_t page_count,
                                struct page_entry *pml4) {
  size_t count = 0;
  while (page_count > 0) {
    int level = __calculate_level(base, page_count);
    size_t shift_count =
        level == PAGE_LEVEL_PAGE ? 1 : (level == PAGE_LEVEL_PT ? 512 : 262144);
    page_count -= shift_count;
    if (!__imap_page((void *)base,
                     PAGE_ENTRY_PRESENT | PAGE_ENTRY_RW | PAGE_ENTRY_GLOBAL,
                     level, pml4)) {
      if (shift_count == 1)
        _kprintf("Failed to identity map page: %p", (void *)base);
      else
        _kprintf("Failed to identity map 0x%x pages: %p", shift_count,
                 (void *)base);
      panic();
    }
    base += shift_count * PAGE_SIZE;
    count += shift_count;
  }

  return count;
}

// Create the identity map
static void __create_identity_map(const uintptr_t new_cr3, size_t entry_count,
                                  struct limine_memmap_entry **entries) {
  for (size_t i = 0; i < entry_count; i++) {
    if (entries[i]->type == LIMINE_MEMMAP_BAD_MEMORY ||
        entries[i]->type == LIMINE_MEMMAP_RESERVED)
      continue;

    const size_t base = entries[i]->base;
    const size_t length = entries[i]->length;
    const size_t moved_base = ROUND_UP(base, PAGE_SIZE);
    const size_t moved_end = ROUND_UP(base + length + 1, PAGE_SIZE) - PAGE_SIZE;
    const size_t page_count = (moved_end - moved_base) / PAGE_SIZE;
    struct page_entry *pml4 = PTV(new_cr3);

    if (__do_identity_map(moved_base, page_count, pml4) != page_count) {
      _kprintf("Failed to map pages\n");
      panic();
    }
  }
}

// x86_64 Memory Map
//
// 0xFFFF800000000000 - 0xFFFFBFFFFFFFFFFF -> Identity Mapped Pages (64 TB)
// 0xFFFFC00000000000 - 0xFFFFC07FFFFFFFFF -> Page Structures (512 GB)
// 0xFFFFC08000000000 - 0xFFFFC0FFFFFFFFFF -> Buddy Data (512 GB)
// 0xFFFFC10000000000 - 0xFFFFFF7FFFFFFFFF -> Unused (62.5 TB)
// 0xFFFFFF8000000000 - 0xFFFFFFFF7FFFFFFF -> Unused (510 GB)
// 0xFFFFFFFF80000000 - 0xFFFFFFFFFFFFFFFF -> Kernel (2 GB)
void __init_page_tables(size_t entry_count,
                        struct limine_memmap_entry **entries) {
  uintptr_t new_cr3 = (uintptr_t)VTP(_fmem_alloc());
  __clone_kernel_mappings(new_cr3);
  __create_identity_map(new_cr3, entry_count, entries);

  // Enable huge pages
  __asm__ volatile("mov %%cr4, %%rax; or $0x10, %%rax; mov %%rax, %%cr4" ::
                       : "rax");
  __asm__ volatile("mov %0, %%cr3" ::"r"(new_cr3) : "memory");
}
