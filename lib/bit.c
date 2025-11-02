#include "lib/bit.h"
#include <stdint.h>

void flip_bit_in_ptr(void *ptr, size_t bit) {
  uint8_t *data_ptr = ptr;
  size_t index = bit >> 3;
  size_t offset = bit & 7;
  data_ptr[index] ^= 1 << offset;
}

bool check_bit_in_ptr(void *ptr, size_t bit) {
  uint8_t *data_ptr = ptr;
  size_t index = bit >> 3;
  size_t offset = bit & 7;
  return data_ptr[index] & (1 << offset);
}
