#include <stdint.h>

void kernel_main() {
    uint16_t* vga = (uint16_t*) (0xb8000);
    *vga = 0x0f4b;
    vga[1] = 0x0f4f;
}