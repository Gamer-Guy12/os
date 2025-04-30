#include <stddef.h>
#include <stdint.h>
#include <x86_64.h>

static uint8_t *global_multiboot;

void init_multiboot(uint8_t *multiboot) { global_multiboot = multiboot; }

uint8_t *get_multiboot(void) { return global_multiboot; }

uint8_t *move_to_type(uint32_t type) {
  uint8_t *multiboot = get_multiboot();
  uint32_t size = multiboot[0] | (multiboot[1] << 8) | (multiboot[2] << 16) |
                  (multiboot[3] << 24);

  // Increment pointer and decrement size as i skip over this part
  multiboot += 8;
  size -= 8;
  uint32_t curr_type = 0;
  uint32_t curr_size = 0;

  while (size > 0) {
    curr_type = multiboot[0] | (multiboot[1] << 8) | (multiboot[2] << 16) |
                (multiboot[3] << 24);

    if (type == curr_type) {
      return multiboot;
    }

    curr_size = multiboot[4] | (multiboot[5] << 8) | (multiboot[6] << 16) |
                (multiboot[7] << 24);

    size -= curr_size;
    multiboot += curr_size;
  }

  return NULL;
}
