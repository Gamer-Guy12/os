#include <libk/mem.h>
#include <stddef.h>
#include <stdint.h>

void memset(void *ptr, uint8_t value, size_t num) {
  uint8_t *newPtr = ptr;
  for (size_t i = 0; i < num; i++) {
    newPtr[i] = value;
  }
}
