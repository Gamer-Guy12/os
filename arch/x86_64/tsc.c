#include "tsc.h"
#include "asm.h"
#include "include/pit.h"
#include "kernel/timers.h"
#include <stdint.h>

static uint64_t tsc_freq = 0;

#define CALIBRATE_MS 4

static void calculate_pit(void) {
  const uint64_t start_ticks = rdtsc();

  const uint16_t ticks = PIT_FREQUENCY * CALIBRATE_MS / 1000;
  pit_wait_ticks(ticks);

  while (read_ticks() != 0) {
  }

  const uint64_t end_ticks = rdtsc();
  const uint64_t ticks_passed = end_ticks - start_ticks;

  // Calculate the frequency by multiply the ticks that passed times the
  // frequency of the pit we just used
  tsc_freq = ticks_passed * 1000 / CALIBRATE_MS;
}

static void calculate_cpuid(void) {
  uint32_t a, b, c, d;
  cpuid_all(0x15, &a, &b, &c, &d);

  tsc_freq = b * c / a;
}

void init_tsc(void) {
  uint32_t a, d;
  cpuid(0, &a, &d);

  if (a >= 0x15) {
    calculate_cpuid();
  } else {
    calculate_pit();
  }
}

uint64_t get_tsc_freq(void) { return tsc_freq; }

uint64_t abs_time(void) { return rdtsc(); }

// What is the timestamp in x ms
uint64_t abs_ms_deadline(uint64_t ms) { return rdtsc() + tsc_freq * ms / 1000; }

uint64_t abs_microsecond_deadline(uint64_t microseconds) {
  return rdtsc() + tsc_freq * microseconds / 1000000;
}

uint64_t abs_freq(void) { return tsc_freq; }
