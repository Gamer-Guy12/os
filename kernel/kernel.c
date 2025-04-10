#include <stdint.h>
#include <libk/kio.h>
#include <libk/string.h>

char* message = "Hello Kernel World!";

void kernel_main(uint8_t* multiboot) {
    // kio_putchar('H');
    // vga_putcharat(0, 0, 'H');
    for (uint64_t i = 0; i < 10; i++) {
        kio_printf("Hello Kernel Loop %x World!\n", i);
    }
}
