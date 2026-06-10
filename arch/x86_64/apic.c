#include "apic.h"
#include "asm.h"
#include "kernel/kprintf.h"
#include "util.h"
#include <stdint.h>

void enable_apic(void) {
  // PIC is already masked so nothing has to be done

  // Set Spurious Interrupt Vector and TPR
  apic_write(LAPIC_TPR_REG, 0);
  // Spurious interrupt is vector 255 and (1 << 8) enables the apic
  apic_write(LAPIC_SVR_REG, 0xFF | (1 << 8));
}

void apic_write(uint16_t reg, uint64_t value) {
  WRMSR(APIC_MSR_BASE + reg, value);
}

uint64_t apic_read(uint16_t reg) { return rdmsr(APIC_MSR_BASE + reg); }

void apic_eoi(void) { apic_write(LAPIC_EOI_REG, 0); }

void apic_ipi(int type, uint32_t dest, uint8_t interrupt) {
  uint64_t data = (uint64_t)dest << 32;
  data |= interrupt;
  // Edge triggered, assert, and physical destination
  switch (type) {
  case IPI_NORMAL:
    data |= (0x0 << 18);
    break;
  case IPI_BROADCAST:
    data |= (0x3 << 18);
    break;
  case IPI_ALL:
    data |= (0x2 << 18);
  default:
    kprintf("Invalid IPI type 0x%x\n", type);
    break;
  }

  apic_write(LAPIC_ICR_REG, data);
}

void init_apic(void) {
  check_apic();
  enable_apic();
}
INITFUNC(init_apic, CALL_CLS);
