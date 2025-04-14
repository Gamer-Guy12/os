#include <libk/kio.h>
#include <libk/kgfx.h>

void kernel_main(void) {
    kgfx_clear();
    kio_printf("We are in Kernel Main!\n");
}
