#include <stddef.h>
#include <stdint.h>
#include <libk/kio.h>
#include <libk/string.h>

// Code thanks to nullplan from the osdev wiki https://forum.osdev.org/viewtopic.php?t=57103
// This code calls the global constructors
#pragma region
typedef void initfunc_t(void);
extern initfunc_t *__init_array_start[], *__init_array_end[];

static void handle_init_array(void) {
  size_t nfunc = ((uintptr_t)__init_array_end - (uintptr_t)__init_array_start)/sizeof (initfunc_t *);
  for (initfunc_t **p = __init_array_start; p < __init_array_start + nfunc; p++)
    (*p)();
}
#pragma endregion

__attribute__ ((section(".startup"))) void kernel_start(uint8_t* multiboot) {
    handle_init_array();
    kio_printf("Hello Kernel World!\n");
}
