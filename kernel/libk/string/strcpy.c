#include <libk/string.h>

void strcpy (char* src, char* dest) {
  size_t i = 0;
  do {
    dest[i] = src[i];
  } while (src[i++] != '\0');
}
