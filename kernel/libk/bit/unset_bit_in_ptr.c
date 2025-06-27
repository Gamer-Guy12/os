#include <libk/math.h>
#include <libk/bit.h>
#include <stddef.h>

void unset_bit_in_ptr(void* ptr, uint64_t bit) {
  size_t index = ROUND_DOWN(bit, 8);
  size_t offset = bit - index;
  index = index / 8;

  uint8_t* true_ptr = ptr;

  true_ptr[index] &= ~(1ull << offset);  
}

