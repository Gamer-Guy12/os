#include <apic.h>
#include <stdint.h>

uint32_t read_apic_register(size_t reg) {
  uint32_t *apic_ptr = (uint32_t *)APIC_REGISTERS_ADDR;

  return apic_ptr[reg];
}

void write_apic_register(size_t reg, uint32_t value) {
  uint32_t *apic_ptr = (uint32_t *)APIC_REGISTERS_ADDR;

  apic_ptr[reg] = value;
}
