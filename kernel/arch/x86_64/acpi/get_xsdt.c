#include <mem/pimemory.h>
#include <acpi/acpi.h>
#include <stddef.h>

static XSDT_t *ptr = NULL;

XSDT_t *get_xsdt(void) {
  if (ptr != NULL)
    return ptr;

  XSDP_t *xsdp = get_xsdp();
  /// Give them the memory mapped address
  ptr = (XSDT_t *)(xsdp->xsdt_addr + IDENTITY_MAPPED_ADDR);

  return ptr;
}
