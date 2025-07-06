#include "libk/kgfx.h"
#include "libk/kio.h"
#include "libk/mem.h"
#include "mem/pimemory.h"
#include <stddef.h>
#include <stdint.h>
#include <x86_64.h>
#include <multiboot.h>
#include <libk/vga_kgfx.h>

void smp_start(uint32_t processor_id) {

  kio_printf("Hi\n");
  while (1) {
  }
}
