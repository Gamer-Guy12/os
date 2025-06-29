#ifndef BIT_H
#define BIT_H

#include <decls.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

inline uint8_t find_zero8(uint8_t num) {
  for (size_t i = 0; i < sizeof(uint8_t) * 8; i++) {
    if (!(num & (1 << i))) {
      return i;
    }
  }

  return 0xFF;
}

inline bool CONST check_bit(uint8_t num, uint8_t pos) {
  return (num & 1 << pos) > 0;
}

inline uint64_t CONST set_bits(uint64_t num, uint64_t bits) {
  return num | bits;
}

inline uint64_t CONST unset_bits(uint64_t num, uint64_t bits) {
  return num & ~bits;
}

void set_bit_in_ptr(void* ptr, uint64_t bit);
void unset_bit_in_ptr(void* ptr, uint64_t bit);

// This is only here because I get confused about the ^ operator
#define XOR(first, second) (first ^ second)

#endif
