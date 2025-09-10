#include <libk/mem.h>
#include <stddef.h>
#include <stdint.h>
#include <x86_64.h>

void *memset_plain(void *ptr, int value, size_t num) {
  uint8_t *newPtr = ptr;
  for (size_t i = 0; i < num; i++) {
    newPtr[i] = value;
  }

  return ptr;
}

void *__attribute__((target("avx"))) memset_avx(void *ptr, int value,
                                                size_t num) {
  uint8_t *newPtr = ptr;
  for (size_t i = 0; i < num; i++) {
    newPtr[i] = value;
  }

  return ptr;
}

void *memset(void *ptr, int value, size_t num) {
  if (get_feature_flags() & FEAT_AVX) {
    return memset_avx(ptr, value, num);
  } else {
    return memset_plain(ptr, value, num);
  }
}
