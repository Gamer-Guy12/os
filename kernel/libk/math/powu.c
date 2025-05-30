#include <libk/math.h>

uint64_t CONST math_powu8(uint8_t base, uint8_t exp) {
  if (exp == 1)
    return base;
  return base * math_powu8(base, exp - 1);
}

uint64_t CONST math_powu16(uint16_t base, uint16_t exp) {
  if (exp == 1)
    return base;
  return base * math_powu16(base, exp - 1);
}

uint64_t CONST math_powu32(uint32_t base, uint32_t exp) {
  if (exp == 1)
    return base;
  return base * math_powu32(base, exp - 1);
}

uint64_t CONST math_powu64(uint64_t base, uint64_t exp) {
  if (exp == 1)
    return base;
  return base * math_powu64(base, exp - 1);
}
