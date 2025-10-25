#include "kernel/mem.h"
#include "kernel/kprintf.h"
#include "lib/string.h"
#include "limine.h"
#include "util.h"
#include <stddef.h>
#include <stdint.h>

extern struct zone zones[];

LIMINE_REQUEST static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST, .revision = 0};

LIMINE_REQUEST static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST, .revision = 0};

#ifdef _x86_64_
// Just to force it to use 4 level paging
LIMINE_REQUEST static volatile struct limine_paging_mode_request
    paging_request = {.id = LIMINE_PAGING_MODE_REQUEST,
                      .revision = 0,
                      .mode = LIMINE_PAGING_MODE_DEFAULT,
                      .min_mode = LIMINE_PAGING_MODE_DEFAULT,
                      .max_mode = LIMINE_PAGING_MODE_DEFAULT};
#else
#error "Cannot find architecture: no paging mode"
#endif

void init_mem(void) {
  kprintf("%p\n", hhdm_request.response->offset);

  for (uint64_t i = 0; i < memmap_request.response->entry_count; i++) {
    uint64_t base = memmap_request.response->entries[i]->base;
    uint64_t length = memmap_request.response->entries[i]->length;
    uint64_t type = memmap_request.response->entries[i]->type;
    kprintf("Index: %u, Base: 0x%x, Length: 0x%x, Type: 0x%x\n", i, base,
            length, type);
  }
  
}
