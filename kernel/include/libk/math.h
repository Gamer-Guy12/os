#ifndef MATH_H
#define MATH_H

#include <stdint.h>

uint64_t math_powu8(uint8_t base, uint8_t exp);
uint64_t math_powu16(uint16_t base, uint16_t exp);
uint64_t math_powu32(uint32_t base, uint32_t exp);
uint64_t math_powu64(uint64_t base, uint64_t exp);

int64_t math_powi8(int8_t base, int8_t exp);
int64_t math_powi16(int16_t base, int16_t exp);
int64_t math_powi32(int32_t base, int32_t exp);
int64_t math_powi64(int64_t base, int64_t exp);

double math_powf(float base, float exp);
double math_powd(double base, double exp);

#define ROUND_UP(num, to) (num + (to - (num % to)))
#define ROUND_DOWN(num, to) (num - (num % to))

#endif
