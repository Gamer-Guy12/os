#include "init.h"
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

struct thread *thread1;
struct thread *thread2;

NORETURN void test1(void) {
  kprintf("Hello 1\n");
  switch_threads(thread1, thread2);
  kprintf("Hello 1 2\n");
  switch_threads(thread1, thread2);
  while (1) {
  }
}

NORETURN void test2(void) {
  kprintf("Hello 2\n");
  switch_threads(thread2, thread1);
  kprintf("Hello 2 2\n");
  switch_threads(thread2, thread1);
  while (1) {
  }
}

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

  init_threading();
  kprintf("[INIT] Initialized Threading\n");

  arch_init_single();

  kprintf("[INIT] Starting Up All Cores\n");
  init_cores();
  kprintf("[INIT] Initialized All Cores\n");

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
