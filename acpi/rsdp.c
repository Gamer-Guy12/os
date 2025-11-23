#include "acpi.h"
#include "kernel/kprintf.h"
#include "limine.h"
#include "util.h"
#include <stddef.h>

static volatile struct limine_rsdp_request rsdp_request = {
    .revision = 0, .id = LIMINE_RSDP_REQUEST};

static struct rsdp *rsdp = NULL;
static struct xsdt *xsdt = NULL;

void init_rsdp(void) {
  rsdp = (struct rsdp *)rsdp_request.response->address;
  xsdt = (struct xsdt *)rsdp->xsdt;
}

void check_rsdp(struct rsdp *rsdp) {
  // Currently only supports 64 bit acpi
  if (rsdp->revision < 2) {
    kprintf("Invalid RSDP Version 1\n");
    panic();
  }

  uint8_t *checksum_bytes = (void *)rsdp;
  uint8_t current_sum = 0;

  for (int i = 0; i < 20; i++) {
    current_sum += checksum_bytes[i];
  }

  if (current_sum != 0) {
    kprintf("RSDP Small Checksum Failed\n");
    panic();
  }

  current_sum = 0;

  for (int i = 0; i < sizeof(struct rsdp); i++) {
    current_sum += checksum_bytes[i];
  }

  if (current_sum != 0) {
    kprintf("RSDP Large Checksum Failed\n");
    panic();
  }
}

struct xsdt *get_xsdt(void) { return xsdt; }
