#include "acpi.h"

void init_acpi(void) {
  init_rsdp();
  init_xsdt();
}

