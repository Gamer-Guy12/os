#include <libk/bit.h>
#include <libk/math.h>
#include <stddef.h>

void set_bit_in_ptr(void *ptr, uint64_t bit) {
  uint8_t *num_ptr = ptr;

  size_t index = ROUND_DOWN(bit, 8);
  size_t offset = bit - index;
  index = index / 8;

  num_ptr[index] |= 1 << offset;
}
