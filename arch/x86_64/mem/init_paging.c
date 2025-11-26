#include "./paging.h"
#include "kernel/kprintf.h"
#include "kernel/mem.h"
#include "lib/string.h"
#include "limine.h"
#include "util.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

LIMINE_REQUEST static volatile struct limine_paging_mode_request
    paging_mode_request = {.id = LIMINE_PAGING_MODE_REQUEST,
                           .revision = 0,
                           .mode = LIMINE_PAGING_MODE_DEFAULT,
                           .min_mode = LIMINE_PAGING_MODE_DEFAULT,
                           .max_mode = LIMINE_PAGING_MODE_DEFAULT};

// Only used temporarily, contains phys addr
static INIT_DATA void *cr3 = NULL;

static INIT uintptr_t phys_to_virt(void *phys) {
  return (uintptr_t)phys + IDENTITY_MAP_OFFSET;
}

#define PML4_INDEX(addr) (((uintptr_t)(addr) >> 39) & 0x1ff)
#define PDPT_INDEX(addr) (((uintptr_t)(addr) >> 30) & 0x1ff)
#define PDT_INDEX(addr) (((uintptr_t)(addr) >> 21) & 0x1ff)
#define PT_INDEX(addr) (((uintptr_t)(addr) >> 12) & 0x1ff)
#define ADDR_MASK 0x0007FFFFFFFFF000ull

// Size of 0 means 4kb page
// Size of 1 means 2mb page
// Size of 2 means 1gb page
static INIT void map_page(uintptr_t phys_addr, void *map_addr, uint16_t flags,
                          bool nx, uint8_t size) {
  struct paging_entry *pml4 = (void *)((uintptr_t)cr3 + IDENTITY_MAP_OFFSET);
  const size_t pml4_index = PML4_INDEX(map_addr);

  struct paging_entry *pdpt = NULL;
  const size_t pdpt_index = PDPT_INDEX(map_addr);

  if (pml4[pml4_index].flags & PAGE_ENTRY_PRESENT) {
    const size_t phys_addr = pml4[pml4_index].addr & ADDR_MASK;
    const size_t virt_addr = phys_addr + IDENTITY_MAP_OFFSET;
    pdpt = (void *)virt_addr;
  } else {
    uintptr_t phys_addr = (uintptr_t)fmem_palloc();
    pdpt = (void *)phys_to_virt((void *)phys_addr);
    pml4[pml4_index].addr = phys_addr;
    pml4[pml4_index].flags = flags;
    pml4[pml4_index].nx = nx;
  }

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
    uintptr_t phys_addr = (uintptr_t)fmem_palloc();
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
    uintptr_t phys_addr = (uintptr_t)fmem_palloc();
    pt = (void *)(phys_to_virt((void *)phys_addr));
    pdt[pdt_index].addr = phys_addr;
    pdt[pdt_index].flags = flags;
    pdt[pdt_index].nx = nx;
  }

  pt[pt_index].addr = phys_addr;
  pt[pt_index].flags = flags;
  pt[pt_index].nx = nx;
  return;

map_gb_page:
  flags |= PAGE_ENTRY_HUGE;
  pdpt[pdpt_index].addr = phys_addr;
  pdpt[pdpt_index].flags = flags;
  pdpt[pdpt_index].nx = nx;
  return;

map_mb_page:
  flags |= PAGE_ENTRY_HUGE;
  pdt[pdt_index].addr = phys_addr;
  pdt[pdt_index].flags = flags;
  pdt[pdt_index].nx = nx;
}

static INIT void map_region(uintptr_t start, size_t size, void *map_addr,
                            uint16_t flags, bool nx) {
  const size_t page_count = size / PAGE_SIZE;
  size_t pages_left = page_count;

  while (pages_left > 0) {
    uintptr_t cur_addr = start + (page_count - pages_left) * PAGE_SIZE;
    uintptr_t new_map_addr =
        (uintptr_t)map_addr + (page_count - pages_left) * PAGE_SIZE;

    if (cur_addr % GB == 0 && pages_left >= (1 << 18)) {
      map_page(cur_addr, (void *)new_map_addr, flags, nx, 2);
      pages_left -= (1 << 18);
    } else if (cur_addr % (MB * 2) == 0 && pages_left >= 512) {
      map_page(cur_addr, (void *)new_map_addr, flags, nx, 1);
      pages_left -= 512;
    } else {
      map_page(cur_addr, (void *)new_map_addr, flags, nx, 0);
      pages_left--;
    }
  }
}

static INIT void map_paging_region(struct limine_memmap_entry *entry) {
  uint16_t flags = PAGE_ENTRY_RW | PAGE_ENTRY_PRESENT | PAGE_ENTRY_GLOBAL;

  if (entry->type == LIMINE_MEMMAP_FRAMEBUFFER) {
    flags |= PAGE_ENTRY_WRITE_THROUGH;
  }

  map_region(entry->base, entry->length,
             (void *)(entry->base + IDENTITY_MAP_OFFSET), flags, true);
}

/// Takes in the physical address of a table, spits of its new physical address
///
/// Level 0 is a pt and level 3 is the pml4, you pass in the pml4 and level 3
/// pdpt gets called with level 2
/// pdt gets called with level 1
/// and pt gets called with level 0
///
/// pt will just copy everything into a new thing
static INIT uintptr_t clone_table(uintptr_t table, uint8_t level) {
  struct paging_entry *entries = (void *)(phys_to_virt((void *)table));

  uintptr_t new_table = (uintptr_t)fmem_palloc();
  struct paging_entry *new_entries = (void *)(new_table + IDENTITY_MAP_OFFSET);

  for (int i = 0; i < 512; i++) {
    if ((entries[i].flags & PAGE_ENTRY_HUGE || level == 0) &&
        (entries[i].flags & PAGE_ENTRY_PRESENT)) {
      new_entries[i].addr = entries[i].addr;
    } else if (entries[i].flags & PAGE_ENTRY_PRESENT) {
      new_entries[i].addr = clone_table(entries[i].addr & ADDR_MASK, level - 1);
      new_entries[i].flags = entries[i].flags;
      new_entries[i].nx = entries[i].nx;
    }
  }

  return new_table;
}

static INIT void clone_kernel_mappings(void) {
  struct paging_entry *pml4 = (void *)((uintptr_t)cr3 + IDENTITY_MAP_OFFSET);
  uintptr_t old_cr3 = 0;
  __asm__ volatile("mov %%cr3, %0" : "=r"(old_cr3));
  struct paging_entry *old_pml4 =
      (void *)((uintptr_t)old_cr3 + IDENTITY_MAP_OFFSET);

  pml4[511].addr = clone_table(old_pml4[511].addr & ADDR_MASK, 2);
  pml4[511].flags = PAGE_ENTRY_GLOBAL | PAGE_ENTRY_PRESENT | PAGE_ENTRY_RW;
}

uintptr_t max_addr = 0;

static INIT void
reserve_page_struct_space(size_t entry_count,
                          struct limine_memmap_entry **entries) {
  for (size_t i = 0; i < entry_count; i++) {
    if (entries[i]->type == LIMINE_MEMMAP_USABLE ||
        entries[i]->type == LIMINE_MEMMAP_EXECUTABLE_AND_MODULES ||
        entries[i]->type == LIMINE_MEMMAP_FRAMEBUFFER ||
        entries[i]->type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE ||
        entries[i]->type == LIMINE_MEMMAP_ACPI_RECLAIMABLE) {
      max_addr = entries[i]->base + entries[i]->length;
    }
  }

  const size_t page_count = max_addr / PAGE_SIZE;
  const size_t pages_needed =
      ((page_count * sizeof(struct page) - 1) / PAGE_SIZE + 1);

  for (size_t i = 0; i < pages_needed; i++) {
    const uintptr_t addr = PAGE_STRUCT_ADDR + i * PAGE_SIZE;
    map_page((uintptr_t)fmem_palloc(), (void *)addr,
             PAGE_ENTRY_PRESENT | PAGE_ENTRY_RW | PAGE_ENTRY_GLOBAL, true, 0);
  }

  memset(pages, 0, pages_needed * PAGE_SIZE);

  kprintf("\t[MEM] Reserved %u Pages for Page Structs\n", pages_needed);
}

static INIT void reserve_buddy_space(size_t entry_count,
                                     struct limine_memmap_entry **entries) {
  const size_t page_count = max_addr / PAGE_SIZE;
  const size_t bytes_needed = page_count / 16 * 2;
  const size_t pages_needed = ((bytes_needed - 1) / PAGE_SIZE) + 1;

  for (size_t i = 0; i < pages_needed; i++) {
    const uintptr_t addr = BUDDY_DATA_ADDR + i * PAGE_SIZE;
    map_page((uintptr_t)fmem_palloc(), (void *)addr,
             PAGE_ENTRY_PRESENT | PAGE_ENTRY_RW | PAGE_ENTRY_GLOBAL, true, 0);
  }

  memset((void *)BUDDY_DATA_ADDR, 0, pages_needed * PAGE_SIZE);

  kprintf("\t[MEM] Reserved %u Pages for Buddy Data\n", pages_needed);
}

uintptr_t get_max_addr(void) { return max_addr; }

// Tasks to initialize paging
// Copy over the mapping for the last 512 gb
// Identity Map the pages necessary
//  - This will only map the sections that are usable, executable, framebuffer,
//  bootloader reclaimable, or acpi tables into memory
// Map enough space for the page structures
//
// x86_64 Memory Map:
//
// 0x0000000000000000 - 0x00007FFFFFFFFFFF -> Userspace (128 TB)
// 0xFFFF800000000000 - 0xFFFFBFFFFFFFFFFF -> Identity Mapped Pages (64 TB)
// 0xFFFFC00000000000 - 0xFFFFC07FFFFFFFFF -> Page Structures (512 GB)
// 0xFFFFC08000000000 - 0xFFFFC0FFFFFFFFFF -> Buddy Data (512 GB)
// 0xFFFFC10000000000 - 0xFFFFFF7FFFFFFFFF -> Unused (62.5 TB)
// 0xFFFFFF8000000000 - 0xFFFFFFFF7FFFFFFF -> Unused (510 GB)
// 0xFFFFFFFF80000000 - 0xFFFFFFFFFFFFFFFF -> Kernel (2 GB)
INIT void init_paging(uint64_t map_entry_count,
                      struct limine_memmap_entry **map_entries) {
  cr3 = fmem_palloc();

  clone_kernel_mappings();

  // Identity Map Pages
  for (uint64_t i = 0; i < map_entry_count; i++) {
    struct limine_memmap_entry *entry = map_entries[i];

    if (entry->type == LIMINE_MEMMAP_USABLE ||
        entry->type == LIMINE_MEMMAP_EXECUTABLE_AND_MODULES ||
        entry->type == LIMINE_MEMMAP_FRAMEBUFFER ||
        entry->type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE ||
        entry->type == LIMINE_MEMMAP_ACPI_TABLES ||
        entry->type == LIMINE_MEMMAP_ACPI_NVS ||
        entry->type == LIMINE_MEMMAP_ACPI_RECLAIMABLE) {
      map_paging_region(entry);
    }
  }

  __asm__ volatile("mov %0, %%cr3" ::"r"((uint64_t)cr3));

  reserve_page_struct_space(map_entry_count, map_entries);
  reserve_buddy_space(map_entry_count, map_entries);

  kprintf("\t[MEM] Initialized Paging\n");
}
