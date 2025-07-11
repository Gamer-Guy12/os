#include <libk/math.h>
#include <stdint.h>

int64_t math_absi(int64_t input) {
  if (input > 0) {
    return input;
  } else {
    return -input;
  }
}
