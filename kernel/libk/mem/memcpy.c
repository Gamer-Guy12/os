#include <libk/mem.h>
#include <stddef.h>
#include <stdint.h>

void memcpy(void *src, void *dest, size_t length) {
  for (size_t i = 0; i < length; i++) {
    ((uint8_t *)src)[i] = ((uint8_t *)dest)[i];
  }
}
