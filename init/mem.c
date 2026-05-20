#include "kernel/mem.h"
#include "kernel/fmem.h"
#include "kernel/kprintf.h"
#include "limine.h"
#include "util.h"
#include <stddef.h>
#include <stdint.h>

LIMINE_REQUEST static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST_ID, .revision = 0};

LIMINE_REQUEST static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST_ID, .revision = 0};

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
    if (entry->type != LIMINE_MEMMAP_USABLE)
      continue;

    uintptr_t start = entry->base + IDENTITY_OFFSET;
    uintptr_t end = entry->base + entry->length + IDENTITY_OFFSET;
    _fmem_add_range((void *)start, (void *)end);
  }
}

void init_mem(void) {
  read_memmap();
  init_page_tables(memmap_request.response->entry_count,
                   memmap_request.response->entries);
}
