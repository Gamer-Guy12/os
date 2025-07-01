#include <acpi/acpi.h>
#include <multiboot.h>
#include <stddef.h>

static XSDP_t *ptr = NULL;

XSDP_t *get_xsdp(void) {
  if (ptr != NULL) {
    return ptr;
  }

  uint8_t *found_ptr = multiboot_get_tag(MLTBT_XSDP);

  // + 8 skips the header on the tag
  ptr = (void *)(found_ptr + 8);

  return ptr;
}
