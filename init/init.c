#include "interrupts.h"
#include "kernel/console.h"
#include "kernel/cores.h"
#include "kernel/kprintf.h"
#include "kernel/mem.h"
#include "kernel/threads.h"
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
static NORETURN void kmain(void *);
// clang-format off
NORETURN void kinit(void) {
  BSP {
    // clang-format on
    if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false) {
      panic();
    }
  }
  _disable_interrupts();

  BSP {
    console_init();
    console_clear();
    _kprintf("[INIT] Initialized Console\n");
  }

  BSP {
    init_mem();
    _kprintf("[INIT] Initialized Memory\n");

    do_calls(CALL_MEM);
  }

  AP { _kprintf("[INIT] Starting Core %u Initialization\n", get_core_id()); }
  __switch_stacks(kmain);

  while (true) {
  }
}

static NORETURN void kmain(void *stack) {
  init_cls();
  // Interrupts are disabled just so it is registered in core local storage and
  // so that they don't run enabled
  disable_interrupts();
  kprintf("[INIT] Initialized CLS on core %u\n", get_core_id());
  // Most normal functions can be used here but continue using _alloc_pages
  // Up until here don't use nested interrupts
  do_calls(CALL_CLS);

  init_threading(stack);
  kprintf("[INIT] Initialized Threading on core %u\n", get_core_id());
  do_calls(CALL_THREADS);

  BSP {
    init_cores();
    kprintf("[INIT] Initialized All Cores\n");
  }

  do_calls(CALL_LATE);

  do_preemption();
  enable_interrupts();
  do_calls(CALL_FINAL);

  THREAD_PRIORITY(THREAD_IDLE);
  while (true) {
    schedule();
  }
}
