#include <acpi/acpi.h>
#include <stdint.h>
#include <x86_64.h>

void init_acpi(void) {
  uint8_t *multiboot = get_multiboot();

  uint32_t total_size = multiboot[0] | (multiboot[1] << 8) |
                        (multiboot[2] << 16) | (multiboot[3] << 24);

  // The way it works is total size will be decremented and multiboot will be
  // incremented to move the pointer and the size until there is no size left

  // Remove the 8 bytes from the start of the multiboot info
  total_size -= 8;
  multiboot += 8;

  while (total_size > 0) {
    uint32_t type = multiboot[0] | (multiboot[1] << 8) | (multiboot[2] << 16) |
                    (multiboot[3] << 24);

    if (type != 15) {
      uint32_t size = multiboot[4] | (multiboot[5] << 8) |
                      (multiboot[6] << 16) | (multiboot[7] << 24);

      // Increment pointer and decrement size
      total_size -= size;
      multiboot += size;

      continue;
    }
  }
}
