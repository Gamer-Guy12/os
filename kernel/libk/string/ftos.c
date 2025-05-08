#include <libk/string.h>
#include <stdint.h>
#include <stdbool.h>
#include <libk/itos.h>

///i sure do hopoe this works
///also i hope this is the right place
void math_ftos (float num, char* buf) {
int32_t i = 0;
int32_t placeholder = 0;
while (i < 32) {
  placeholder = (int32_t) num;
  buf = buf + string_itos32(placeholder);
  num = num - placeholder;
  num = num * 10;
  i++;
}
}
