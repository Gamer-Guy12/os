#include <acpi/acpi.h>
#include <libk/string.h>

bool check_header(sdt_header_t *header, char signature[4]) {
  return strcmp(header->signature, signature, 4);
}

void *acpi_get_struct(char signature[4]) {
  XSDT_t *xsdt = get_xsdt();
  size_t ptr_count = (xsdt->sdt_header.length - sizeof(xsdt->sdt_header)) / 8;

  for (size_t i = 0; i < ptr_count; i++) {
    if (check_header((sdt_header_t *)xsdt->sdt_ptrs[i], signature))
      return (void*)xsdt->sdt_ptrs[i];
  }

  return NULL;
}
