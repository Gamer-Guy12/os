#include <kernel.h>
#include <libk/kgfx.h>
#include <libk/kio.h>
#include <libk/lock.h>
#include <libk/string.h>
#include <pmm.h>
#include <stddef.h>
#include <stdint.h>

// Code thanks to nullplan from the osdev wiki
// https://forum.osdev.org/viewtopic.php?t=57103 This code calls the global
// constructors
#pragma region Global Constructors
typedef void initfunc_t(void);
extern initfunc_t *__init_array_start[], *__init_array_end[];

static void handle_init_array(void) {
  size_t nfunc = ((uintptr_t)__init_array_end - (uintptr_t)__init_array_start) /
                 sizeof(initfunc_t *);
  for (initfunc_t **p = __init_array_start; p < __init_array_start + nfunc; p++)
    (*p)();
}
#pragma endregion

void print_multiboot_size(uint8_t *multiboot) {
  uint64_t size = *((uint32_t *)multiboot);
  uint64_t reserved = ((uint32_t *)multiboot)[1];

  kio_printf(
      "The size of the multiboot header is: %x or %u and the reserved is %x\n",
      size, size, reserved);
}

__attribute__((section(".startup"))) void kernel_start(uint8_t *multiboot) {
  handle_init_array();
  kgfx_clear();
  print_multiboot_size(multiboot);
  setup_page_frame_allocation(multiboot);
  pageframe_t frame = allocate_page_frame();
  uint64_t *bigPtr = (uint64_t *)frame;
  *bigPtr = 54;
  kio_printf("The value stored is %u\n", *bigPtr);
  pageframe_t frame2 = allocate_page_frame();
  uint64_t *bigPtr2 = (uint64_t *)frame2;
  *bigPtr2 = 485;
  kio_printf("The value stored again is %u\n", *bigPtr2);
  kio_printf("The value stored is %u\n", *bigPtr);
  free_page_frame(&frame);
  free_page_frame(&frame2);
  frame = allocate_page_frame();
  bigPtr = (uint64_t *)frame;
  *bigPtr = 54;
  kio_printf("The value stored is %u\n", *bigPtr);

  start_cores();
}
