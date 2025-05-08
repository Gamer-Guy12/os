#include <libk/string.h>
#include <stdint.h>
#include <stdbool.h>
#include <libk/itos.h>

///i sure do hopoe this works
///also i hope this is the right place
void math_ftos (float num, char* buf) {
int32_t i = 0;
int32_t placeholder = 0;
placeholder = (int32_t) num; ////"but why dont you just declare the variable at the beginning?" it's cuz i copy and pasted this and i dont care
buf = buf + string_itos32(placeholder);
num = num - placeholder;
num = num * 10;
buf = buf + "." //decimal point!
while (i < 10) { //10 digit accuracy. floats aren't really accurate after this. if you don't like this cry about it
  placeholder = (int32_t) num;
  buf = buf + string_itos32(placeholder);
  num = num - placeholder;
  num = num * 10;
  i++;
}
}
