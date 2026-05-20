#include "paging.h"
#include "kernel/fmem.h"
#include "kernel/kprintf.h"
#include "kernel/mem.h"
#include "limine.h"
#include "util.h"
#include <stddef.h>
#include <stdint.h>

#define ROUND_UP(num, to) ((((num) + (to) - 1) / (to)) * (to))

// Maps a physical address to virtual and returns physical address
//
// Level:
// - PAGE_LEVEL_PAGE - 4Kb
// - PAGE_LEVEL_PT - 2 Mb
// - PAGE_LEVEL_PDT - 1 Gb
// PAGE_ENTRY_HUGE not needed
// PAGE_ENTRY_GLOBAL by default (can't be changed)
//
// Assumes nx
static void *__map_page(void *phys, uint16_t flags, int level,
                        struct page_entry *pml4) {
  // Start traversal
  // Check if pdpt has a pointer to the pdt
  // Check if pdt has pointer to pt
  // Make pt point to phys
  const uintptr_t virt = (uintptr_t)phys + IDENTITY_OFFSET;
  if (level != PAGE_LEVEL_PAGE)
    flags |= PAGE_ENTRY_HUGE;

  // Index into pt
  const size_t page_index = (virt >> 12) & 0x1FF;
  const size_t pt_index = (virt >> 21) & 0x1FF;
  const size_t pdt_index = (virt >> 30) & 0x1FF;
  const size_t pdpt_index = (virt >> 39) & 0x1FF;

  struct page_entry *pdpt = PTV(pml4[pdpt_index].addr & ADDR_MASK);
  struct page_entry *pdt = NULL;
  struct page_entry *pt = NULL;
  struct page_entry *entry = NULL;
  if (level == PAGE_LEVEL_PDT) {
    entry = &pdpt[pdt_index];
    goto map_page;
  }

  if (pdpt[pdt_index].flags & PAGE_ENTRY_PRESENT) {
    pdt = PTV(pdpt[pdt_index].addr & ADDR_MASK);
    goto make_pt;
  }

  // Create entry
  pdpt[pdt_index].addr = (uintptr_t)VTP(_fmem_alloc());
  pdpt[pdt_index].nx = 1;
  pdpt[pdt_index].flags =
      PAGE_ENTRY_PRESENT | PAGE_ENTRY_RW | PAGE_ENTRY_GLOBAL;
  pdt = PTV(pdpt[pdt_index].addr & ADDR_MASK);

  if (level == PAGE_LEVEL_PT) {
    entry = &pdt[pt_index];
    goto map_page;
  }
make_pt:
  if (pdt[pt_index].flags & PAGE_ENTRY_PRESENT) {
    pt = PTV(pdt[pt_index].addr & ADDR_MASK);
    entry = &pt[page_index];
    goto map_page;
  }

  // Create entry
  pdt[pt_index].addr = (uintptr_t)VTP(_fmem_alloc());
  pdt[pt_index].nx = 1;
  pdt[pt_index].flags = PAGE_ENTRY_PRESENT | PAGE_ENTRY_RW | PAGE_ENTRY_GLOBAL;
  pt = PTV(pdt[pt_index].addr & ADDR_MASK);
  entry = &pt[page_index];

map_page:
  entry->addr = (uintptr_t)phys;
  entry->flags = flags;
  entry->nx = 1;

  return (void *)virt;
}

// Inputs
// Level of the inputted entry (if it is part of the pml4 it should be
// PAGE_LEVEL_PML4) For example when inputing a pointer to the pml4 level should
// be PAGE_LEVEL_PML4 Pointer to (virtual) page that contains the table that
// should be copied Pointer to (virtual) page that contains the table that
// should be copied to
static void __clone_mappings(struct page_entry *src, struct page_entry *dst,
                             const int level) {
  // If the table contains raw data (e.g. either level = PAGE_LEVEL_PT or
  // PAGE_ENTRY_HUGE) then the data should be copied elsewise the table should
  // be remade
  for (int i = 0; i < 512; i++) {
    if (level == PAGE_LEVEL_PT || src[i].flags & PAGE_ENTRY_HUGE) {
      dst[i] = src[i];
    } else {
      // Create new table
      dst[i].addr = (uintptr_t)VTP(_fmem_alloc());
      dst[i].nx = src[i].nx;
      dst[i].flags = src[i].flags;

      __clone_mappings(PTV(src[i].addr & ADDR_MASK),
                       PTV(dst[i].addr & ADDR_MASK), level - 1);
    }
  }
}

static void __create_top_pages(struct page_entry *pml4) {
  for (int i = 256; i < 512; i++) {
    pml4[i].addr = (uintptr_t)VTP(_fmem_alloc());
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
  __clone_mappings(PTV(old_pml4[511].addr & ADDR_MASK),
                   PTV(new_pml4[511].addr & ADDR_MASK), PAGE_LEVEL_PDPT);
}

// Checks how many bits are unset at the bottom
// Depending on the amount it tells the alignment
// Then checks if there are enough pages left
static int __calculate_level(size_t base, size_t page_count) {
  // Checking if the bottom 30 bits are set
  if ((base & ~(0x3FFFFFFF)) == 0 && page_count >= 262144) {
    return PAGE_LEVEL_PDT;
  } else if ((base & ~(0x1FFFFF)) == 0 && page_count >= 512) {
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
    __map_page((void *)base, PAGE_ENTRY_PRESENT | PAGE_ENTRY_RW, level, pml4);
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
    const size_t moved_end = ROUND_UP(base + length, PAGE_SIZE);
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
void init_page_tables(size_t entry_count,
                      struct limine_memmap_entry **entries) {
  uintptr_t new_cr3 = (uintptr_t)VTP(_fmem_alloc());
  __clone_kernel_mappings(new_cr3);
  __create_identity_map(new_cr3, entry_count, entries);
}
