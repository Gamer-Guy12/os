#include <acpi.h>
#include <asm.h>
#include <libk/kio.h>
#include <stdint.h>
#include <x86_64.h>

void init_apic(void) {
  XSDP_t *xsdp = get_xsdp();
  XSDT_t *xsdt = find_xsdt(xsdp);
  MADT_t *madt = find_madt(xsdt);

  uint64_t apic_base = rdmsr(0x1B);

  uint64_t low = apic_base & 0xffffffff;

  uint64_t base = 0;

  // Stop clangd from complaining about unused variables when no PME
  uint64_t high = 0;
  high = high * 2;

#ifdef PME
  high = (apic_base >> 32) & 0xffffffff;
  base = (low & 0xfffff000) | ((high & 0x0f) << 32);
#else
  base = (low & 0xfffff000);
#endif
  low = (base & 0xfffff0000) | 0x800;

#ifdef PME
  high = 0;
  high = (base >> 32) & 0xf;
#endif

  kio_printf("The value that is being written is %u while the base is %u\n",
             (high << 32) | low, apic_base);
  wrmsr(0x1B, (high << 32) | low);

  ((uint8_t *)(uint64_t)madt->lapic_addr)[0xF0] =
      ((uint8_t *)(uint64_t)madt->lapic_addr)[0xF0] | 0x100;
}
