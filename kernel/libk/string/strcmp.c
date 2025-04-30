#include <libk/string.h>

bool strcmp(char *str1, char *str2, size_t length) {
  for (size_t i = 0; i < length; i++) {
    if (str1[i] != str2[i])
      return false;
  }

  return true;
}
