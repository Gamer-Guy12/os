#include "init.h"
#include "acpi.h"
#include "kernel/console.h"
#include "kernel/cores.h"
#include "kernel/kprintf.h"
#include "kernel/mem.h"
#include "kernel/threads.h"
#include "kernel/timers.h"
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

    static NORETURN void kmain(void *new_stack);

void handler(void *data) {}

// clang-format off
INIT NORETURN void kinit(void) {
  // clang-format on
  if (LIMINE_BASE_REVISION_SUPPORTED == false) {
    panic();
  }

  console_init();
  kprintf("[INIT] Initialized Console\n");

  kprintf("[INIT] Starting Memory Initialization\n");
  init_mem();
  kprintf("[INIT] Initialized Memory\n");

  // Starting stack switch
  __switch_stacks(kmain);

  while (1) {
  }
}

static NORETURN void kmain(void *new_stack) {
  init_cls();
  kprintf("[INIT] Initialized CLS\n");

  BSP {
    init_general_threading();
    kprintf("[INIT] Initialized General Threading\n");
  }

  init_threading(new_stack);
  kprintf("[INIT] Initialized Threading on Core %u\n", get_core_id());

  BSP {
    init_acpi();
    kprintf("[INIT] Initialized ACPI\n");

    init_timers();
    kprintf("[INIT] Initialized Timers\n");
  }

  arch_init();

  BSP {
    kprintf("[INIT] Starting Up All Cores\n");
    init_cores();
    kprintf("[INIT] Initialized All Cores\n");

    int_in_ms(20000, handler, NULL);
    int_in_ms(8000, handler, NULL);
    int_in_ms(9000, handler, NULL);
    int_in_ms(15000, handler, NULL);
    int_in_ms(15000, handler, NULL);
    int_in_ms(15333, handler, NULL);
  }

  // This thread will be the idle thread
  while (true) {
    schedule();
  }
}

NORETURN void core_entry(void) {
  kprintf("[INIT] Starting Core %u Initialization\n", get_core_id());
  __switch_stacks(kmain);

  while (1) {
  }
}
