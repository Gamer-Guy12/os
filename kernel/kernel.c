#include <stdint.h>
#include <libk/kio.h>
#include <libk/vga.h>

char* message = "Hello Kernel World!";

void kernel_main(uint8_t* multiboot) {
    // kio_putchar('H');
    // vga_putcharat(0, 0, 'H');
    kio_puts(message);
}
