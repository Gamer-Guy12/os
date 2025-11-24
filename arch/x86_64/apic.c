#include "apic.h"
#include "asm.h"
#include "kernel/kprintf.h"
#include "util.h"
#include <stdbool.h>
#include <stdint.h>

#define IA32_APIC_BASE_MSR 0x1B

bool check_apic(void) {
  uint32_t a, d;
  cpuid(1, &a, &d);
  return d & (1 << 9);
}

void enable_apic(void) {
  if (!check_apic()) {
    kprintf("No Apic On Chip\n");
    panic();
  }

  // Enable with apic base address
  WRMSR(IA32_APIC_BASE_MSR, LAPIC_BASE | (1 << 11));

  
}
