#include "pit.h"
#include "include/asm.h"
#include "interrupts.h"
#include <stdint.h>

void init_pit(void) {
  // Since im not using interupts or anything it should be fine just not to use it
}

void pit_wait_ticks(uint16_t ticks) {
  disable_interrupts();
  WRITE_PIT_COMMAND(PIT_16_BINARY | PIT_CHANNEL_0 | PIT_LOHIBYTE | PIT_MODE_0);
  IO_WAIT();
  WRITE_PIT_DATA_0(ticks & 0xFF);
  IO_WAIT();
  WRITE_PIT_DATA_0(ticks >> 8);
  IO_WAIT();
  enable_interrupts();
}

uint16_t read_ticks(void) {
  disable_interrupts();

  WRITE_PIT_COMMAND(PIT_LATCH_COUNT_VAL | PIT_CHANNEL_0);
  IO_WAIT();
  uint16_t count = 0;

  count |= inb(PIT_DATA_0);
  count |= inb(PIT_DATA_0) << 8;

  enable_interrupts();

  return count;
}

uint16_t ms_to_ticks(uint8_t ms) { return (ms * PIT_FREQUENCY) / 1000; }
