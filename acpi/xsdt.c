#include "acpi.h"
#include "kernel/mem.h"
#include "kernel/kprintf.h"
#include "lib/string.h"
#include "util.h"
#include <stddef.h>
#include <stdint.h>

static struct xsdt *xsdt = NULL;

void init_xsdt(void) {
  xsdt = get_xsdt();

  uint8_t *bytes = (void *)xsdt;
  uint8_t cur_sum = 0;

  for (int i = 0; i < xsdt->header.length; i++) {
    cur_sum += bytes[i];
  }

  if (cur_sum != 0) {
    kprintf("Invalid Checksum for XSDT\n");
    panic();
  }
}

void *get_acpi_table(char *signature) {
  uint32_t entry_count = (xsdt->header.length - sizeof(xsdt->header)) / 8;

  for (uint32_t i = 0; i < entry_count; i++) {
    struct sdt_header *header = (struct sdt_header *)(xsdt->sdts[i] + IDENTITY_MAP_OFFSET);
    if (strncmp(header->signature, signature, 4) == 0) {
      return (void *)header;
    }
  }

  return NULL;
}
