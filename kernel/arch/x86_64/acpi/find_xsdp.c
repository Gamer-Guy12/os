#include "libk/kio.h"
#include <acpi.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static bool check_for_type(uint8_t *multiboot, uint32_t desiredType) {
  uint32_t type = (multiboot[0]) | (multiboot[1] << 8) | (multiboot[2] << 16) |
                  (multiboot[2] << 24);

  // return type == false;
  return type == desiredType;
}

static XSDP_t *xsdp;

XSDP_t *get_xsdp(void) { return xsdp; }

XSDP_t *find_xsdp(uint8_t *multiboot) {
  uint8_t *rsdp = NULL;
  // Increment the pointer to skip over until you get to the start of the RSDP
  // info struct
  multiboot += 8;
  while (!check_for_type(multiboot, 15)) {

    if (((multiboot[0]) | (multiboot[1] << 8) | (multiboot[2] << 16) |
         (multiboot[2] << 24)) == 0) {
      kio_printf("End of header\n");
      multiboot = rsdp;
      break;
    }

    if (((multiboot[0]) | (multiboot[1] << 8) | (multiboot[2] << 16) |
         (multiboot[2] << 24)) == 14) {
      rsdp = multiboot;
    }

    uint32_t size = (multiboot[4]) | (multiboot[5] << 8) |
                    (multiboot[6] << 16) | (multiboot[7] << 24);
    if (size % 8 != 0)
      size += (8 - (size % 8));

    multiboot += size;
  }

  // Skip over the header part of the struct
  multiboot += 8;

  xsdp = (XSDP_t *)multiboot;
  return (XSDP_t *)multiboot;
}
