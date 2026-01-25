#include "kernel/timers.h"
#include <stddef.h>
#include <stdint.h>

static uint64_t (*read_time_cb)(void) = NULL;
static uint64_t (*read_freq_cb)(void) = NULL;
static uint64_t cb_freq = 0;

void register_abs_timer(uint64_t (*read_time)(void),
                        uint64_t (*read_freq)(void), uint64_t freq) {
  if (freq < cb_freq)
    return;
  cb_freq = freq;

  read_time_cb = read_time;
  read_freq_cb = read_freq;
}

uint64_t abs_time(void) { return read_time_cb(); }

uint64_t abs_ms_deadline(uint64_t ms) {
  return read_time_cb() + read_freq_cb() * ms / 1000;
}

uint64_t abs_microsecond_deadline(uint64_t microseconds) {
  return read_time_cb() + read_freq_cb() * microseconds / 1000000;
}

uint64_t abs_freq(void) { return read_freq_cb(); }
