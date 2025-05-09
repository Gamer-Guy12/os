#include "libk/math.h"
#include <stddef.h>
#include <stdint.h>
#include <x86_64.h>

static uint8_t *global_multiboot;

void init_multiboot(uint8_t *multiboot) { global_multiboot = multiboot; }

uint8_t *get_multiboot(void) { return global_multiboot; }

size_t get_multiboot_size(void) {
  uint8_t *multiboot = get_multiboot();
  multiboot += 8;

  return *(uint32_t *)multiboot;
}

uint8_t *move_to_type(uint32_t type) {
  uint8_t *multiboot = get_multiboot();
  uint32_t size = *(uint32_t *)multiboot;

  // Skip over static part of information
  multiboot += 8;
  size -= 8;

  while (size > 0) {
    uint32_t cur_type = *(uint32_t *)multiboot;

    if (cur_type == type) {
      return multiboot;
    }

    uint32_t cur_size = ROUND_UP(((uint32_t *)multiboot)[1], 8);

    if (size < cur_size)
      break;

    size -= cur_size;
    multiboot += cur_size;
  }

  return NULL;
}
