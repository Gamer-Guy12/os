// ASSUMES INVARIENT TSC

#ifndef _x86_64_TSC_H_
#define _x86_64_TSC_H_

#include <stdint.h>

static inline uint64_t rdtsc(void) {
  uint64_t a, d;
  __asm__ volatile("rdtsc" : "=a"(a), "=d"(d));
  return (d << 32) | a;
}

void init_tsc(void);
uint64_t get_tsc_freq(void);

#endif
