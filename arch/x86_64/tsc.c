#include "tsc.h"
#include "asm.h"
#include <stdint.h>

static uint64_t tsc_freq = 0;

static void calculate_pit(void) {

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

