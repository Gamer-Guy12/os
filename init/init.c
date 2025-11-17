#include "init.h"
#include "kernel/console.h"
#include "kernel/cores.h"
#include "kernel/kprintf.h"
#include "kernel/mem.h"
#include "limine.h"
#include "util.h"
#include <stdbool.h>
#include <stddef.h>

#define LIMINE_SECTION(name) __attribute__((used, section(name)))

// clang-format off
LIMINE_SECTION(".limine_requests") static volatile LIMINE_BASE_REVISION(4)

LIMINE_SECTION(".limine_requests_start") static volatile LIMINE_REQUESTS_START_MARKER

LIMINE_SECTION(".limine_requests_end") static volatile LIMINE_REQUESTS_END_MARKER
    // clang-format on

    // clang-format off
NORETURN void kinit(void) {
  // clang-format on
  if (LIMINE_BASE_REVISION_SUPPORTED == false) {
    panic();
  }

  console_init();
  kprintf("[INIT] Initialized Console\n");

  kprintf("[INIT] Starting Memory Initialization\n");
  init_mem();
  kprintf("[INIT] Initialized Memory\n");

  init_cls();
  kprintf("[INIT] Initialized CLS\n");

  arch_init();

  kprintf("[INIT] Starting Up All Cores\n");
  init_cores();
  kprintf("[INIT] Initialized All Cores\n");

  kprintf("Hello Kernel World!\n");

  while (1) {
  }
}

NORETURN INIT void core_entry(void) {
  kprintf("[INIT] Starting Core %u Initialization\n", get_core_id());

  init_cls();
  kprintf("[INIT] Initialized CLS on Core %u\n", get_core_id());

  arch_init();

  while (1) {
  }
}
