#include "init.h"
#include "kernel/console.h"
#include "kernel/cores.h"
#include "kernel/kprintf.h"
#include "limine.h"
#include "util.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define LIMINE_SECTION(name) __attribute__((used, section(name)))

// clang-format off
LIMINE_SECTION(".limine_requests") static volatile LIMINE_BASE_REVISION(4)

LIMINE_SECTION(".limine_requests_start") static volatile LIMINE_REQUESTS_START_MARKER

LIMINE_SECTION(".limine_requests_end") static volatile LIMINE_REQUESTS_END_MARKER
    // clang-format on
    static NORETURN void kmain(void);
// clang-format off
INIT NORETURN void kinit(void) {
  BSP {
    // clang-format on
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
      panic();
    }
  }

  BSP {
    console_init();
    kprintf("[INIT] Initialized Console\n");
  }

  AP { kprintf("[INIT] Starting Core %u Initialization\n", get_core_id()); }
  kmain();

  while (1) {
  }
}

static NORETURN void kmain(void) {
  arch_init();

  BSP {
    init_cores();
    kprintf("[INIT] Initialized All Cores\n");
  }

  while (true) {
  }
}
