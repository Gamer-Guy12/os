#include "lib/string.h"
#include <stddef.h>
#include <stdint.h>

void *memcpy(void *dest, const void *src, size_t n) {
  uint8_t *dest_ptr = dest;
  const uint8_t *src_ptr = src;

  for (size_t i = 0; i < n; i++) {
    dest_ptr[i] = src_ptr[i];
  }

  return dest;
}

void *memset(void *ptr, int v, size_t n) {
  uint8_t *dest = ptr;

  for (size_t i = 0; i < n; i++) {
    dest[i] = v;
  }

  return ptr;
}

void *memmove(void *dest, const void *src, size_t n) {
  uint8_t *dest_ptr = dest;
  const uint8_t *src_ptr = src;

  if (src > dest) {
    for (size_t i = 0; i < n; i++) {
      dest_ptr[i] = src_ptr[i];
    }
  } else {
    for (size_t i = n; i > 0; i--) {
      dest_ptr[i - 1] = src_ptr[i - 1];
    }
  }

  return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  const uint8_t *p1 = s1;
  const uint8_t *p2 = s2;

  for (size_t i = 0; i < n; i++) {
    if (p1[i] != p2[i]) {
      return p1[i] < p2[i] ? -1 : 1;
    }
  }

  return 0;
}

size_t strlen(const char *s) {
  size_t index = 0;
  while (s[index++] != '\0') {
  }
  return index - 1;
}

char *strcpy(char *dest, const char *src) {
  size_t len = strlen(src) + 1;
  return memcpy(dest, src, len);
}

char *strrev(char *str) {
  size_t len = strlen(str);
  size_t i, j;
  char temp;

  for (i = 0, j = len - 1; i < j; i++, j--) {
    temp = str[i];
    str[i] = str[j];
    str[j] = temp;
  }

  return str;
}
