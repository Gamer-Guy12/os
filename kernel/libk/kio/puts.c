#include <libk/kio.h>

void kio_puts(char* c) {
    uint16_t index = 0;

    while (c[index] != '\0') {
        kio_putchar(c[index]);
        index++;
    }
}
