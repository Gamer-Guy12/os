#ifndef BIT_H
#define BIT_H

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

#endif
