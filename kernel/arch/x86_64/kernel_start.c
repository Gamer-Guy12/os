#include "hal/vimemory.h"
#include <hal/memory.h>
#include <hal/pimemory.h>
#include <libk/bst.h>
#include <libk/kgfx.h>
#include <libk/kio.h>
#include <libk/spinlock.h>
#include <libk/string.h>
#include <stddef.h>
#include <stdint.h>
#include <x86_64.h>

extern void kernel_main(void);

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

void kernel_start(uint8_t *multiboot) {
  handle_init_array();
  kio_printf("Called Global Constructors\n");
  init_multiboot(multiboot);
  kio_printf("Initialized Multiboot\n");
  init_memory_manager();
  kio_printf("Initialized Memory Manager\n");
  // kio_clear();

  size_t phys_1 = (size_t)phys_alloc();
  phys_free((void *)phys_1);
  size_t phys_2 = (size_t)phys_alloc();
  size_t phys_3 = (size_t)phys_alloc();
  phys_free((void *)phys_2);
  size_t phys_4 = (size_t)phys_alloc();
  phys_free((void *)phys_3);
  phys_free((void *)phys_4);

  extern char end_kernel[];

  kio_printf("Addr 1 %x, 2 %x, 3 %x, 4 %x diff %x\n", phys_1, phys_2, phys_3,
             phys_4, (size_t)(void *)end_kernel - KERNEL_CODE_OFFSET - phys_1);

  uint64_t *page = map_page((void *)GB, PT_READ_WRITE, 0);
  *page = 48;
  map_page((void *)(GB + PAGE_SIZE), PT_READ_WRITE, 1);

  kio_printf("Addr %x, value %x\n", (size_t)page, *page);
  unmap_page((void *)GB);
  unmap_page((void *)(GB + PAGE_SIZE));

  vmm_kernel_region_t region;
  create_kernel_region(&region);

  increment_kernel_brk(&region, 0x4001);
  uint64_t *num = increment_kernel_brk(&region, 0);
  *num = 49;

  kio_printf("Num %u\n", *num);

  decrement_kernel_brk(&region, 0x4001);
  // Uncomment to make the kernel fault to show that moving the break backwards
  // unmaps the pages
  //*num = 49;
  //
  // kio_printf("Num %u\n", *num);

  kernel_main();
}
