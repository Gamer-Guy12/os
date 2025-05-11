///varunan whyyy
/// does not currently support float as a exp. it probably could but its 11:30 and i'm very tired
#include <libk/math.h>

int64_t math_powf8(float base, int8_t exp) {
    if (exp == 1) return base;
    return base * math_powf8(base, exp - 1);
}
///tf does this algorithm even do
int64_t math_powf16(float base, int16_t exp) {
    if (exp == 1) return base;
    return base * math_powf16(base, exp - 1);
}

int64_t math_powf32(float base, int32_t exp) {
    if (exp == 1) return base;
    return base * math_powf32(base, exp - 1);
}

int64_t math_powf64(float base, int64_t exp) {
    if (exp == 1) return base;
    return base * math_powf64(base, exp - 1);
}
