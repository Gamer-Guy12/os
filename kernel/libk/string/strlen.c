#include <libk/string.h>

uint16_t strlen(char *buf) {
  uint16_t index = 0;
  while (buf[index] != '\0')
    index++;

  return index;
}
