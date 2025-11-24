#include "apic.h"
#include "asm.h"
#include "kernel/kprintf.h"
#include "kernel/mem.h"
#include "util.h"
#include <stdbool.h>
#include <stdint.h>

#define IA32_APIC_BASE_MSR 0x1B

static uintptr_t lapic_addr = 0;

INIT bool check_apic(void) {
  uint32_t a, d;
  cpuid(1, &a, &d);
  return d & (1 << 9);
}

INIT void enable_apic(void) {
  if (!check_apic()) {
    kprintf("No Apic On Chip\n");
    panic();
  }

  // Enable with apic base address
  WRMSR(IA32_APIC_BASE_MSR, LAPIC_BASE | (1 << 11));

  lapic_addr =
      (uintptr_t)map_phys((void *)0xFEE00000, PM_WRITE_THROUGH | PM_RW);

  // Enable the apic in the SVR
  // The vector will be 0xFF
  write_apic_reg(LAPIC_SVR_REG, 0xFF | (1 << 8));
  write_apic_reg(LAPIC_TPR_REG, 0);
}

void write_apic_reg(uint16_t reg, uint32_t val) {
  volatile uint32_t *write_addr = (void *)(lapic_addr + reg);

  *write_addr = val;
}

uint32_t read_apic_reg(uint16_t reg) {
  volatile uint32_t *read_addr = (void *)(lapic_addr + reg);
  return *read_addr;
}
