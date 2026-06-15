#include "pit.h"
#include "asm.h"
#include "lib/spinlock.h"
#include "util.h"
#include <stdbool.h>
#include <stdint.h>

static SPINLOCK(pit_lock);
uint64_t ticks_left = 0;

#define MIN(a, b) ((a) < (b) ? (a) : (b))

// Starts a countdown (safe for multicore)
void pit_prep(uint16_t ms) {
  // calculate the tick count
  uint64_t ticks = (uint64_t)ms * PIT_FREQ / 1000;
  // Lock
  spinlock_acquire(&pit_lock);
  // Save the tick count
  ticks_left = ticks;
}

// Ends the countdown by waiting
void pit_count(void) {
  while (ticks_left > 0) {
    uint16_t ticks = MIN(MAX_16, ticks_left);
    ticks_left -= ticks;
    // Get the commands set up
    OUTB(PIT_COMMAND, PIT_BINARY | PIT_MODE0 | PIT_LOHIBYTE | PIT_SELECT0);
    OUTB(PIT_CHANNEL_0, ticks & MAX_8);
    OUTB(PIT_CHANNEL_0, ticks >> 8);

    do {
      OUTB(PIT_COMMAND, 0xE2);
    } while (!(inb(PIT_CHANNEL_0) & (1 << 7)));
  }

  spinlock_release(&pit_lock);
}
