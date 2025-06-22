#include <libk/math.h>
#include <stdint.h>

int64_t math_log(int64_t input, int64_t base) {
  // TODO: add detection to make sure it is actually a power of the base (not
  // too important right now)
  int64_t answer = 0;
  while (input > 1) {
    input = input / base;
    answer++;
  }
  return answer;
}
