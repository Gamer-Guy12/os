#include <acpi.h>
#include <libk/string.h>
#include <stddef.h>

MADT_t *find_madt(XSDT_t *xsdt) {
  size_t entry_count = (xsdt->header.length - sizeof(xsdt->header)) / 8;

  for (size_t i = 0; i < entry_count; i++) {
    char apic_sig[4] = {'A', 'P', 'I', 'C'};
    if (strcmp(xsdt->pointer[i]->signature, apic_sig, 4)) {
      return (MADT_t *)xsdt->pointer[i];
    }
  }

  return NULL;
}
