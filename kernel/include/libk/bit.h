#ifndef BIT_H
#define BIT_H

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

inline bool check_bit(uint8_t num, uint8_t pos) { return (num & 1 << pos) > 0; }

inline uint64_t set_bits(uint64_t num, uint64_t bits) { return num | bits; }
inline uint64_t unset_bits(uint64_t num, uint64_t bits) { return num & ~bits; }

#endif
