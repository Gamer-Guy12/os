#include <x86_64.h>
#include <libk/mem.h>
#include <stddef.h>
#include <stdint.h>

void memcpy_plain(void *src, void *dest, size_t length) {
  for (size_t i = 0; i < length; i++) {
    ((uint8_t *)dest)[i] = ((uint8_t *)src)[i];
  }
}

void __attribute__((target("avx"))) memcpy_avx(void *src, void *dest,
                                               size_t length) {
  for (size_t i = 0; i < length; i++) {
    ((uint8_t *)dest)[i] = ((uint8_t *)src)[i];
  }
}

void memcpy(void *src, void *dest, size_t length) {
  if (get_feature_flags() & FEAT_AVX) {
    memcpy_avx(src, dest, length);
  } else {
    memcpy_plain(src, dest, length);
  }
}
