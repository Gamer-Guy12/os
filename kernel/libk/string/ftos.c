#include <libk/string.h>
#include <stdint.h>
#include <stdbool.h>
#include <libk/itos.h>
#include <libk/stcmp.h>
#include <libk/strlen.h>

///i sure do hopoe this works
///also i hope this is the right place
void math_ftos (float num, char* buf) {
int32_t i = 0;
int32_t placeholder = 0;
int length = 0;
char* p = ""
placeholder = (int32_t) num; ////"but why dont you just declare the variable at the beginning?" it's cuz i copy and pasted this and i dont care
string_itos32(placeholder, p);
buf = p;
length = length(p) + 1;
num = num - placeholder;
num = num * 10;
*buf = buf + '.'; //decimal point!
while (length < 20) { //this is wayyy more digits than we need and/or is accurate.
  placeholder = (int32_t) num;
  string_itos32(placeholder, p);
  buf[length] = p;
  num = num - placeholder;
  num = num * 10;
  length++;
}
buf[length] = '\0'
}
