#include "init.h"
#include "acpi.h"
#include "interrupts.h"
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

void entry(void) {
  kprintf("Here %x %x\n", get_cur_thread()->tid, get_core_id());
  wait();
  kprintf("Here %x %x\n", get_cur_thread()->tid, get_core_id());
  terminate(0);
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
  tid_t t3 = 0;

  BSP {
    tid_t t1 = create_thread(entry, TP_NORMAL)->tid;
    tid_t t2 = create_thread(entry, TP_INTERRUPT)->tid;
    t3 = create_thread(entry, TP_INTERRUPT)->tid;
    kprintf("%x\n", t1);
    kprintf("%x\n", t2);
    kprintf("%x\n", t3);
    schedule();
  }

  // This thread will be the idle thread
  get_cur_thread()->priority = TP_IDLE;
  BSP {
    kprintf("awaken\n");
    awaken_thread(t3);
  }
  while (true) {
    BSP { schedule(); }
  }
}
