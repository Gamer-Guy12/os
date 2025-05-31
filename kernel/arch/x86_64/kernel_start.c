#include <hal/imemory.h>
#include <libk/kgfx.h>
#include <libk/kio.h>
#include <libk/lock.h>
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
  kio_printf("I might be in!\n");
  kernel_main();
}
