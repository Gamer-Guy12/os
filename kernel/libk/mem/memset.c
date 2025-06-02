#include <libk/mem.h>
#include <stddef.h>
#include <stdint.h>

void *memset(void *ptr, int value, size_t num) {
  uint8_t *newPtr = ptr;
  for (size_t i = 0; i < num; i++) {
    newPtr[i] = value;
  }

  return ptr;
}
