#include "apic.h"
#include "libk/kio.h"
#include <asm.h>
#include <hal/irq.h>
#include <stddef.h>
#include <stdint.h>

#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_BSP 0x100
#define IA32_APIC_BASE_MSR_ENABLE 0x800

bool check_apic(void) {
  uint32_t a = 0;
  uint32_t d = 0;

  cpuid(0x1, &a, &d);

  if (d & (1 << 9)) {
    return true;
  }

  return false;
}

uintptr_t get_apic_base(void) {
  uint64_t a, d;
  uint64_t value = rdmsr(IA32_APIC_BASE_MSR);

  a = value & 0xffffffff;
  d = (value >> 32) & 0xffffffff;

#ifdef PME
  return (a & 0xfffff000) | ((d & 0x0f) << 32);
#else
  return (a & 0xfffff000);
#endif
}

void set_apic_base(uintptr_t apic) {
  uint32_t d = 0;
  uint32_t a = (apic & 0xfffff0000) | IA32_APIC_BASE_MSR_ENABLE;

#ifdef PME
  d = (apic >> 32) & 0x0f;
#endif

  wrmsr(IA32_APIC_BASE_MSR, a | ((size_t)d << 32));
}

hal_irq_t init_apic(void) {
  set_apic_base(get_apic_base());

  // 0x1ff is split into 0xff and 0x100
  // 0xff is the interrupt number for it
  // 0x100 just says to start interrupts
  write_apic_register(SPURIOUS_INTERRUPT_VECTOR_REG, 0x1ff);

  hal_irq_t irq = {NULL};
  return irq;
}
