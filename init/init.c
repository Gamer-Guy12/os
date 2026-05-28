#include "init.h"
#include "kernel/console.h"
#include "kernel/cores.h"
#include "kernel/kprintf.h"
#include "kernel/mem.h"
#include "limine.h"
#include "util.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define LIMINE_SECTION(name) __attribute__((used, section(name)))

// clang-format off
LIMINE_SECTION(".limine_requests") static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(6);

LIMINE_SECTION(".limine_requests_start") static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

LIMINE_SECTION(".limine_requests_end") static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;
// clang-format on
static NORETURN void kmain(void);
// clang-format off
INIT NORETURN void kinit(void) {
  BSP {
    // clang-format on
    if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false) {
      panic();
    }
  }

  BSP {
    console_init();
    _kprintf("[INIT] Initialized Console\n");
  }

  BSP {
    init_mem();
    _kprintf("[INIT] Initialized Memory\n");
  }

  AP { _kprintf("[INIT] Starting Core %u Initialization\n", get_core_id()); }
  kmain();

  while (1) {
  }
}

static NORETURN void kmain(void) {
  arch_init();

  BSP {
    init_cores();
    _kprintf("[INIT] Initialized All Cores\n");
  }

  BSP {
    void *ptr = _alloc_page(ZONE_ANY);
    void *dma_ptr = _alloc_page(ZONE_DMA);

    _kprintf("Normal: %p, DMA: %p\n", ptr, dma_ptr);
  }

  while (true) {
  }
}
