#include <libk/math.h>
double math_sqrtdfast(double input) {
    union { uint64_t i; double d; } u = { .d = input };
    u.i = (u.i >> 1) + (0x3fe0000000000000ULL >> 1);
    double y = u.d;
    y = 0.5 * (y + input / y);
    y = 0.5 * (y + input / y);
    y = 0.5 * (y + input / y);
    return y;
}