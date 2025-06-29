#include <multiboot.h>
#include <acpi/acpi.h>
#include <stddef.h>

XSDP_t* ptr = NULL;

XSDP_t* get_xsdp(void) {
  if (ptr != NULL) {
    return ptr;
  }

  uint8_t* found_ptr = multiboot_get_tag(MLTBT_XSDP);

  // + 8 skips the header on the tag
  ptr = (void*)(found_ptr + 8);

  return ptr;
}

