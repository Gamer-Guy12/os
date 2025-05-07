#include <libk/string.h>
#include <stdint.h>
#include <stdbool.h>
#include <libk/itos.h>

///i sure do hopoe this works
///also i hope this is the right place
void math_ftos (float num, char* buf) {
int i = 0;
int placeholder = 0;
while (i < 32) {
  int placeholder = int(buf)
  buf[i] = placeholder;
  num = num - placeholder;
  num = num * 10;
  i++;
}


return num
}
