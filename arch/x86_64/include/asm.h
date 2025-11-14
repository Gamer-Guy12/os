#ifndef _x86_64_ASM_H_
#define _x86_64_ASM_H_

#include "util.h"
#include <stddef.h>
#include <stdint.h>

static inline uint64_t rdmsr(uint64_t msr) {
  uint32_t low, high;
  __asm__ volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));

  return ((uint64_t)high << 32 | low);
}

static inline void wrmsr(uint64_t msr, uint64_t value) {
  uint32_t low = value & MAX_32;
  uint32_t high = value >> 32;
  __asm__ volatile("wrmsr" ::"c"(msr), "a"(low), "d"(high));
}

static inline void cpuid_all(int code, uint32_t *a, uint32_t *b, uint32_t *c,
                             uint32_t *d) {
  uint32_t a_temp, b_temp, c_temp, d_temp;

  if (a == NULL) {
    a = &a_temp;
  }

  if (b == NULL) {
    b = &b_temp;
  }

  if (c == NULL) {
    c = &c_temp;
  }

  if (d == NULL) {
    d = &d_temp;
  }

  __asm__ volatile("cpuid"
                   : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
                   : "0"(code));
}

#define CPUID_AD(code, a, d) cpuid_all(code, a, NULL, NULL, d)

#define MFENCE __asm__ volatile("mfence" ::: "memory")

#endif
