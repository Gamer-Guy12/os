#include <acpi.h>

XSDT_t *find_xsdt(XSDP_t *xsdp) { return xsdp->xsdt_addr; }
