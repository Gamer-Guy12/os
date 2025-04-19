#include "libk/kio.h"
#include <io.h>
#include <libk/bit.h>
#include <stddef.h>
#include <x86_64.h>

static inline void wait_ms(uint32_t ms) {
  for (size_t i = 0; i < ms; i++) {
    outb(0x43, 0x30);
    io_wait();
    outb(0x40, 0xA9);
    io_wait();
    outb(0x40, 0x4);
    io_wait();

    do {
      outb(0x43, 0xE2);
    } while (!check_bit(inb(0x40), 7));
  }
}

void start_cores(uint8_t *multiboot) {
  kio_printf("%u bit\n", check_bit(2, 0));
  wait_ms(10000);
  kio_printf("Hello Wait\n");
}
