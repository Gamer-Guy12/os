#include "kernel/cls.h"
#include "kernel/console.h"
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

  kprintf("[INIT] Starting CLS Initialization\n");
  init_cls();
  kprintf("[INIT] Initialized CLS\n");

  kprintf("Hello Kernel World!\n");

  while (1) {
  }
}
