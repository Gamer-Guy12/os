//// how has this not be made
#include <libk/string.h>
#include <libk/strlen.h>

void strcpy (char* src, char* dest) {
  uint16_t i = 0;
  do {
    src[i] = dest[i];
    i++;
  } while (src[i] != '\0');
}
