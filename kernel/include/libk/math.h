#ifndef MATH_H
#define MATH_H

#include <decls.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

uint64_t WUNUSED CONST math_powu8(uint8_t base, uint8_t exp);
uint64_t WUNUSED CONST math_powu16(uint16_t base, uint16_t exp);
uint64_t WUNUSED CONST math_powu32(uint32_t base, uint32_t exp);
uint64_t WUNUSED CONST math_powu64(uint64_t base, uint64_t exp);

int64_t WUNUSED CONST math_powi8(int8_t base, int8_t exp);
int64_t WUNUSED CONST math_powi16(int16_t base, int16_t exp);
int64_t WUNUSED CONST math_powi32(int32_t base, int32_t exp);
int64_t WUNUSED CONST math_powi64(int64_t base, int64_t exp);

double WUNUSED CONST math_powf(float base, float exp);
double WUNUSED CONST math_powd(double base, double exp);

int64_t WUNUSED CONST math_absi(int64_t input);
float WUNUSED CONST math_absf(float input);

int64_t WUNUSED CONST math_log(int64_t input, int64_t base);

float WUNUSED CONST math_sqrtffast(float x);
double WUNUSED CONST math_sqrtdfast(double input);

float WUNUSED CONST math_sqrtf(float input);
double WUNUSED CONST math_sqrtd(double input);

#define ROUND_DOWN(num, to) (((num) / (to)) * (to))
#define ROUND_UP(num, to) ((((num) + ((to) - 1)) / (to)) * (to))

// 128 bit operations
typedef struct {
  uint64_t lower;
  uint64_t upper;
} uint128_t;

uint128_t add_128(uint128_t lhs, uint128_t rhs);
uint128_t sub_128(uint128_t lhs, uint128_t rhs);
uint128_t mul_128(uint64_t lhs, uint64_t rhs);
uint128_t div_128(uint128_t lhs, uint64_t rhs);
bool equals_128(uint128_t lhs, uint128_t rhs);
bool less_than_128(uint128_t lhs, uint128_t rhs);

#define IS_ZERO_128(val) equals_128(val, (uint128_t){.lower = 0, .upper = 0})
#define ZERO_128 (uint128_t){.lower = 0, .upper = 0}
#define NUM_128(val) (uint128_t){.lower = val, .upper = 0}

#endif
