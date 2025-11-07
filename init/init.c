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

  uint64_t *ptr = alloc_pages(0, ALLOC_KERNEL);
  kprintf("%p 0\n", ptr);
  free_pages(ptr, 0);
  ptr = alloc_pages(0, ALLOC_KERNEL);
  uint64_t *ptr2 = alloc_pages(0, ALLOC_KERNEL);
  kprintf("%p %p 1\n", ptr, ptr2);
  free_pages(ptr, 0);
  free_pages(ptr2, 0);
  ptr = alloc_pages(0, ALLOC_KERNEL);
  ptr2 = alloc_pages(0, ALLOC_KERNEL);
  uint64_t *ptr3 = alloc_pages(0, ALLOC_KERNEL);
  uint64_t *ptr4 = alloc_pages(0, ALLOC_KERNEL);
  uint64_t *ptr5 = alloc_pages(0, ALLOC_KERNEL);
  uint64_t *ptr6 = alloc_pages(0, ALLOC_KERNEL);
  uint64_t *ptr7 = alloc_pages(0, ALLOC_KERNEL);
  kprintf("%p %p %p %p %p %p %p\n", ptr, ptr2, ptr3, ptr4, ptr5, ptr6, ptr7);

  kprintf("Hello Kernel World!\n");

  while (1) {
  }
}
