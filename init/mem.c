#include "kernel/mem.h"
#include "kernel/fmem.h"
#include "kernel/kprintf.h"
#include "lib/spinlock.h"
#include "limine.h"
#include "util.h"
#include <stddef.h>
#include <stdint.h>

#define ROUND_UP(num, to) ((((num) + (to) - 1) / (to)) * (to))

LIMINE_REQUEST static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST_ID, .revision = 0};

LIMINE_REQUEST static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST_ID, .revision = 0};

static size_t max_addr = 0;

void read_memmap(void) {
  // We are picky and only accept if the identity offset is equal to
  // IDENTITY_OFFSET
  if (hhdm_request.response->offset != IDENTITY_OFFSET) {
    _kprintf("Cannot load fmem memory when offset does not equal "
             "IDENTITY_OFFSET: 0x%x\n",
             hhdm_request.response->offset);
    panic();
  }

  struct limine_memmap_response *memmap_response = memmap_request.response;

  for (size_t i = 0; i < memmap_response->entry_count; i++) {
    struct limine_memmap_entry *entry = memmap_response->entries[i];

    uintptr_t start = entry->base + IDENTITY_OFFSET;
    uintptr_t end = entry->base + entry->length + IDENTITY_OFFSET;
    if (end - IDENTITY_OFFSET > max_addr) {
      if (entry->type == LIMINE_MEMMAP_USABLE ||
          entry->type == LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE ||
          entry->type == LIMINE_MEMMAP_ACPI_RECLAIMABLE) {
        max_addr = end - IDENTITY_OFFSET;
      }
    }

    if (entry->type != LIMINE_MEMMAP_USABLE)
      continue;
    start = ROUND_UP(start, PAGE_SIZE);
    end = ROUND_UP(end + 1, PAGE_SIZE) - PAGE_SIZE;
    _fmem_add_range((void *)start, (void *)end);
  }
}

// Returns page count
size_t calculate_mem_sizes(void) {
  size_t total = 0;
  for (int i = 0; i < ZONE_COUNT; i++) {
    struct zone *zone = __get_zone(i);
    // No more memory
    if ((uintptr_t)zone->base > max_addr)
      break;

    if (zone->length == 0) {
      zone->length = max_addr - (uintptr_t)zone->base;
    }

    const size_t page_count = zone->length / PAGE_SIZE;
    total += page_count;
    size_t page_index = 0;
    // Byte count (16 pages per byte rounded up)
    size_t byte_count = ROUND_UP(page_count, 16) / 16;
    for (int i = 0; i < MAX_ORDER; i++) {
      const size_t page_count = ROUND_UP(byte_count, PAGE_SIZE) / PAGE_SIZE;
      for (size_t j = 0; j < page_count; j++) {
        const uintptr_t addr = BUDDY_OFFSET + PAGE_SIZE * (page_index + j);
        if (!__map_page((void *)addr, MAP_RW | MAP_NX | MAP_PINNED,
                        _fmem_phys)) {
          _kprintf("Failed to alloc page for buddy\n");
          panic();
        }
      }
      zone->buddy[i].data = (void *)(BUDDY_OFFSET + PAGE_SIZE * page_index);
      page_index += page_count;
      byte_count = ROUND_UP(byte_count, 2) / 2;
    }
  }

  return total;
}

static void clean_buddy_memory(void) {
  for (int i = 0; i < ZONE_COUNT; i++) {
    struct zone *zone = __get_zone(i);
    for (int j = 0; j < MAX_ORDER; j++) {
      // Make circular
      zone->buddy[j].freelist.next = &zone->buddy[j].freelist;
      zone->buddy[j].freelist.prev = &zone->buddy[j].freelist;
    }

    // Create lock
    switch (i) {
    case ZONE_DMA:
      zone->lock = (spinlock_t)SPINLOCK_ZERO(dma_lock);
      break;
    case ZONE_NORMAL:
      zone->lock = (spinlock_t)SPINLOCK_ZERO(normal_lock);
      break;
    default:
      zone->lock = (spinlock_t)SPINLOCK_ZERO(zone_lock);
      break;
    }
  }
}

static void allocate_page_structs(size_t count) {
  size_t total_size = sizeof(struct page) * count;
  size_t page_count = ROUND_UP(total_size, PAGE_SIZE) / PAGE_SIZE;

  for (size_t i = 0; i < page_count; i++) {
    uintptr_t addr = PAGE_STRUCT_OFFSET + i * PAGE_SIZE;
    if (!__map_page((void *)addr, MAP_RW | MAP_NX | MAP_PINNED, _fmem_phys)) {
      _kprintf("Failed to allocate memory for page structs\n");
      panic();
    }
  }
}

void init_mem(void) {
  read_memmap();
  __init_page_tables(memmap_request.response->entry_count,
                     memmap_request.response->entries);
  allocate_page_structs(calculate_mem_sizes());
  clean_buddy_memory();
  free_fmem();
}
