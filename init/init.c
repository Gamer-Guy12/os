#include "init.h"
#include "acpi.h"
#include "interrupts.h"
#include "kernel/console.h"
#include "kernel/cores.h"
#include "kernel/kprintf.h"
#include "kernel/mem.h"
#include "kernel/threads.h"
#include "kernel/timers.h"
#include "lib/spinlock.h"
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
spinlock_t lock = SPINLOCK_ZERO;
// clang-format off
INIT NORETURN void kinit(void) {
  BSP {
    // clang-format on
    if (LIMINE_BASE_REVISION_SUPPORTED == false) {
      panic();
    }
  }

  disable_interrupts();

  BSP {
    console_init();
    kprintf("[INIT] Initialized Console\n");

    init_mem();
    kprintf("[INIT] Initialized Memory\n");
  }

  AP { kprintf("[INIT] Starting Core %u Initialization\n", get_core_id()); }
  // Starting stack switch
  __switch_stacks(kmain);

  while (1) {
  }
}

struct thread *main_thread;
struct thread *second_thread;

void entry(void) {
  kprintf("Here\n");
  switch_threads(second_thread, main_thread);
}

static NORETURN void kmain(void *new_stack) {
  init_cls();
  disable_interrupts();

  init_threading(new_stack);

  BSP {
    init_acpi();
    init_timers();
  }

  arch_init();

  BSP {
    init_cores();
    kprintf("[INIT] Initialized All Cores\n");
  }
  enable_interrupts();

  BSP {
    main_thread = get_cur_thread();
    second_thread = create_thread(entry);
    switch_threads(main_thread, second_thread);
    destroy_thread(second_thread);
  }

  // This thread will be the idle thread
  while (true) {
  }
}

