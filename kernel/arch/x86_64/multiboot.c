#include <libk/kio.h>
#include <libk/math.h>
#include <multiboot.h>
#include <stddef.h>
#include <stdint.h>
#include <x86_64.h>

static uint8_t *global_multiboot;

typedef struct {
  uint32_t type;
  /// Make sure to round up by 8
  uint32_t size;
} multiboot_tag_header_t;

void *multiboot_get_tag(uint32_t type) {
  uint8_t *ptr = get_multiboot();

  uint32_t total_size = *(uint32_t *)ptr;
  // The static part before the header has the size of 8 bytes so - 8 skips over
  // those
  uint32_t size_remaining = total_size - 8;

  while (size_remaining > 0) {
    multiboot_tag_header_t *cur_header =
        (multiboot_tag_header_t *)(ptr + total_size - size_remaining);

    if (cur_header->type == type) {
      return (ptr + total_size - size_remaining);
    }

    uint32_t true_size = ROUND_UP(cur_header->size, 8);

    if (true_size > size_remaining || cur_header->type == 0) {
      break;
    }

    size_remaining -= true_size;
  }

  return NULL;
}

void print_multiboot_info(void) {
  uint8_t *ptr = get_multiboot();

  uint32_t total_size = *(uint32_t *)ptr;
  // The static part before the header has the size of 8 bytes so - 8 skips over
  // those
  uint32_t size_remaining = total_size - 8;

  while (size_remaining > 0) {
    multiboot_tag_header_t *cur_header =
        (multiboot_tag_header_t *)(ptr + total_size - size_remaining);

    kio_printf("Multiboot: Type %u, Size %x\n", cur_header->type,
               cur_header->size);

    uint32_t true_size = ROUND_UP(cur_header->size, 8);

    if (true_size > size_remaining || cur_header->type == 0) {
      break;
    }

    size_remaining -= true_size;
  }
}

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
