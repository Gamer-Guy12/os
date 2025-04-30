#include <libk/math.h>

int64_t math_powi8(int8_t base, int8_t exp) {
    if (exp == 1) return base;
    return base * math_powi8(base, exp - 1);
}

int64_t math_powi16(int16_t base, int16_t exp) {
    if (exp == 1) return base;
    return base * math_powi16(base, exp - 1);
}

int64_t math_powi32(int32_t base, int32_t exp) {
    if (exp == 1) return base;
    return base * math_powi32(base, exp - 1);
}

int64_t math_powi64(int64_t base, int64_t exp) {
    if (exp == 1) return base;
    return base * math_powi64(base, exp - 1);
}
