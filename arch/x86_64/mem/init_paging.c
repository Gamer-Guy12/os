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

static INIT void map_region(uintptr_t start, size_t size, void *map_addr,
                            uint16_t flags, bool nx) {
  size_t page_count = size / PAGE_SIZE;

  /// How many 1 gb pages
  size_t massive_page_count = page_count / (1 << 18);
  page_count -= massive_page_count * (1 << 18);
  /// How many 2 mb pages
  size_t huge_page_count = page_count / 512;
  page_count -= huge_page_count * 512;
  size_t normal_page_count = page_count;

  for (size_t i = 0; i < massive_page_count; i++) {
    uintptr_t base_addr = start + i * GB;

    size_t pml4_index = PML4_INDEX(map_addr);
    struct paging_entry *pml4 = (struct paging_entry *)(phys_to_virt(cr3));

    struct paging_entry *pdpt = NULL;

    if (pml4[pml4_index].flags & PAGE_PRESENT) {
      uintptr_t phys_addr = pml4[pml4_index].addr & ADDR_MASK;
      uintptr_t virt_addr = phys_addr + IDENTITY_MAP_OFFSET;
      pdpt = (struct paging_entry *)(virt_addr);
    } else {
      void *phys_addr = fmem_palloc();
      pdpt = (void *)phys_to_virt(phys_addr);
      pml4[pml4_index].addr = (uintptr_t)phys_addr;
      pml4[pml4_index].flags = flags;
      pml4[pml4_index].nx = nx;
    }

    // Each index represents 1 GB
    size_t pdpt_index = PDPT_INDEX(map_addr);
    pdpt[pdpt_index].addr = base_addr;
    pdpt[pdpt_index].flags = flags | PAGE_HUGE;
    pdpt[pdpt_index].nx = nx;
  }

  for (size_t i = 0; i < huge_page_count; i++) {
    uintptr_t base_addr = start + massive_page_count * GB + i * 2 * MB;

    size_t pml4_index = PML4_INDEX(map_addr);
    struct paging_entry *pml4 = (struct paging_entry *)(phys_to_virt(cr3));

    struct paging_entry *pdpt = NULL;

    if (pml4[pml4_index].flags & PAGE_PRESENT) {
      uintptr_t phys_addr = pml4[pml4_index].addr & ADDR_MASK;
      uintptr_t virt_addr = phys_addr + IDENTITY_MAP_OFFSET;
      pdpt = (struct paging_entry *)(virt_addr);
    } else {
      void *phys_addr = fmem_palloc();
      pdpt = (void *)phys_to_virt(phys_addr);
      pml4[pml4_index].addr = (uintptr_t)phys_addr;
      pml4[pml4_index].flags = flags;
      pml4[pml4_index].nx = nx;
    }

    size_t pdpt_index = PDPT_INDEX(map_addr);

    struct paging_entry *pdt = NULL;

    if (pdpt[pdpt_index].flags & PAGE_PRESENT) {
      uintptr_t phys_addr = pdpt[pdpt_index].addr & ADDR_MASK;
      uintptr_t virt_addr = phys_addr + IDENTITY_MAP_OFFSET;
      pdt = (struct paging_entry *)(virt_addr);
    } else {
      void *phys_addr = fmem_palloc();
      pdt = (void *)phys_to_virt(phys_addr);
      pdpt[pdpt_index].addr = (uintptr_t)phys_addr;
      pdpt[pdpt_index].flags = flags;
      pdpt[pdpt_index].nx = nx;
    }

    // Each Index represents 2 mb
    size_t pdt_index = PDT_INDEX(map_addr);
    pdt[pdt_index].addr = base_addr;
    pdt[pdt_index].flags = flags | PAGE_HUGE;
    pdt[pdt_index].nx = nx;
  }

  for (size_t i = 0; i < normal_page_count; i++) {
    uintptr_t base_addr = start + massive_page_count * GB +
                          huge_page_count * MB * 2 + i * PAGE_SIZE;

    size_t pml4_index = PML4_INDEX(map_addr);
    struct paging_entry *pml4 = (struct paging_entry *)(phys_to_virt(cr3));

    struct paging_entry *pdpt = NULL;

    if (pml4[pml4_index].flags & PAGE_PRESENT) {
      uintptr_t phys_addr = pml4[pml4_index].addr & ADDR_MASK;
      uintptr_t virt_addr = phys_addr + IDENTITY_MAP_OFFSET;
      pdpt = (struct paging_entry *)(virt_addr);
    } else {
      void *phys_addr = fmem_palloc();
      pdpt = (void *)phys_to_virt(phys_addr);
      pml4[pml4_index].addr = (uintptr_t)phys_addr;
      pml4[pml4_index].flags = flags;
      pml4[pml4_index].nx = nx;
    }

    size_t pdpt_index = PDPT_INDEX(map_addr);

    struct paging_entry *pdt = NULL;

    if (pdpt[pdpt_index].flags & PAGE_PRESENT) {
      uintptr_t phys_addr = pdpt[pdpt_index].addr & ADDR_MASK;
      uintptr_t virt_addr = phys_addr + IDENTITY_MAP_OFFSET;
      pdt = (struct paging_entry *)(virt_addr);
    } else {
      void *phys_addr = fmem_palloc();
      pdt = (void *)phys_to_virt(phys_addr);
      pdpt[pdpt_index].addr = (uintptr_t)phys_addr;
      pdpt[pdpt_index].flags = flags;
      pdpt[pdpt_index].nx = nx;
    }

    size_t pdt_index = PDT_INDEX(map_addr);

    struct paging_entry *pt = NULL;

    if (pdt[pdt_index].flags & PAGE_PRESENT) {
      uintptr_t phys_addr = pdt[pdt_index].addr & ADDR_MASK;
      uintptr_t virt_addr = phys_addr + IDENTITY_MAP_OFFSET;
      pt = (struct paging_entry *)(virt_addr);
    } else {
      void *phys_addr = fmem_palloc();
      pt = (void *)phys_to_virt(phys_addr);
      pdt[pdt_index].addr = (uintptr_t)phys_addr;
      pdt[pdt_index].flags = flags;
      pdt[pdt_index].nx = nx;
    }

    // Each Index represents 2 mb
    size_t pt_index = PT_INDEX(map_addr);
    pt[pt_index].addr = base_addr;
    pt[pt_index].flags = flags;
    pt[pt_index].nx = nx;
  }
}

static INIT void map_paging_region(struct limine_memmap_entry *entry) {
  uint16_t flags = PAGE_RW | PAGE_PRESENT | PAGE_GLOBAL;

  if (entry->type == LIMINE_MEMMAP_FRAMEBUFFER) {
    flags |= PAGE_WRITE_THROUGH;
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
    if (entries[i].flags & PAGE_HUGE || level == 0) {
      new_entries[i] = entries[i];
    } else {
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

  pml4[511].addr = clone_table(old_cr3, 2);
  pml4[511].flags = PAGE_GLOBAL | PAGE_PRESENT;
}

// Tasks to initialize paging
// Copy over the mapping for the last 512 gb
// Identity Map the pages necessary
//  - This will only map the sections that are usable, executable, framebuffer,
//  bootloader reclaimable, or acpi tables into memory
// Map enough space for the page structures
//
// x86_64 Memory Map:
//
// 0x0 - 0x7FFFFFFFFFFF -> Userspace (128 TB)
// 0xFFFF800000000000 - 0xFFFFBFFFFFFFFFFF -> Identity Mapped Pages (64 TB)
// 0xFFFFC00000000000 - 0xFFFFC07FFFFFFFFF -> Page Structures (512 GB)
// 0xFFFFC08000000000 - 0xFFFFFF7FFFFFFFFF -> Unused (63 TB)
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
        entry->type == LIMINE_MEMMAP_ACPI_TABLES) {
      map_paging_region(entry);
    }
  }

  __asm__ volatile("mov %0, %%cr3" ::"r"((uint64_t)cr3));

  kprintf("\t[MEM] Initialized Paging\n");
}
