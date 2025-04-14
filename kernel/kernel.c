#include <libk/kgfx.h>
#include <libk/kio.h>

void kernel_main(void) {
  kgfx_clear();
  kio_printf("We are in Kernel Main!\n");
}
