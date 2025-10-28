#include "kernel/mem.h"
#include "kernel/kprintf.h"
#include "limine.h"
#include "util.h"
#include <stddef.h>

LIMINE_REQUEST static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST, .revision = 0};

LIMINE_REQUEST static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST, .revision = 0};

static uint64_t memmap_entry_count = 0;
static struct limine_memmap_entry **memmap_entries = NULL;

void init_mem(void) {
  init_fmem(hhdm_request.response->offset);
  kprintf("\t[MEM] Initialized FMem\n");

  memmap_entry_count = memmap_request.response->entry_count;
  memmap_entries = memmap_request.response->entries;
  kprintf("\t[MEM] Map Entry Count: %u\n", memmap_entry_count);
  uint64_t usable_region_count = 0;

  for (uint64_t i = 0; i < memmap_entry_count; i++) {
    if (memmap_entries[i]->type == LIMINE_MEMMAP_USABLE) {
      fmem_pfree_range(
          (void *)memmap_entries[i]->base,
          (void *)(memmap_entries[i]->base + memmap_entries[i]->length + 1));
      usable_region_count++;
    }
  }
  kprintf("\t[MEM] Freed Usable Regions: %u\n", usable_region_count);

  kprintf("%p\n", fmem_palloc());
}
