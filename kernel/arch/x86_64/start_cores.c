#include <acpi.h>
#include <asm.h>
#include <libk/bit.h>
#include <libk/kio.h>
#include <libk/lock.h>
#include <libk/mem.h>
#include <stddef.h>
#include <stdint.h>
#include <x86_64.h>

extern uint16_t ap_tramp;
extern uint16_t ap_len;
volatile uint8_t aps_running = 0;

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

static lock_t starting_cores;

void start_cores(uint8_t *multiboot) {

  lock_acquire(&starting_cores);

  // Pause a second to stop clangd from complaining about uncalled function
  wait_ms(1000);

  lock_release(&starting_cores);
}
