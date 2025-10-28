#include "./paging.h"
#include "kernel/kprintf.h"
#include "kernel/mem.h"
#include "limine.h"
#include "util.h"
#include <stddef.h>

LIMINE_REQUEST static volatile struct limine_paging_mode_request
    paging_mode_request = {.id = LIMINE_PAGING_MODE_REQUEST,
                           .revision = 0,
                           .mode = LIMINE_PAGING_MODE_DEFAULT,
                           .min_mode = LIMINE_PAGING_MODE_DEFAULT,
                           .max_mode = LIMINE_PAGING_MODE_DEFAULT};

static INIT void map_region(uintptr_t start, size_t size, void *map_addr, uint16_t flags) {}

static INIT void map_paging_region(struct limine_memmap_entry *entry) {
  uint16_t flags = PAGE_RW | PAGE_PRESENT | PAGE_GLOBAL;

  if (entry->type == LIMINE_MEMMAP_FRAMEBUFFER) {
    flags |= PAGE_WRITE_THROUGH;
  }

  map_region(entry->base, entry->length,
             (void *)(entry->base + IDENTITY_MAP_OFFSET), flags);
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
  // Map the kernel into memory

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

  kprintf("\t[MEM] Initialized Paging\n");
}
