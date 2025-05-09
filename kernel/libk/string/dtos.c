#include <libk/string.h>
#include <stdint.h>
#include <stdbool.h>
#include <libk/itos.h>
#include <libk/stcmp.h>
#include <libk/strlen.h>

///i sure do hopoe this works
///also i hope this is the right place
void string_dtos (double num, char* buf) {
int32_t i = 0;
int32_t placeholder = 0;
int length = 0;
char p[20] = "";
placeholder = (int32_t) num; ////"but why dont you just declare the variable at the beginning?" it's cuz i copy and pasted this and i dont care
string_itos32(placeholder, p);
strcpy(buf, p);
length = strlen(p);
num = num - placeholder;
num = num * 10;
buf[length++] = '.'; //decimal point!
while (length < 32) { //this is wayyy more digits than we need and/or is accurate.
  placeholder = (int32_t) num;
  if (placeholder < 0) placeholder *= -1;
  string_itos32(placeholder, p);
  buf[length] = p[0];
  num = num - placeholder;
  num = num * 10;
  length++;
}
buf[length] = '\0';
}
