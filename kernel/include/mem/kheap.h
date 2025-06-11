#ifndef KHEAP_H
#define KHEAP_H

#include <stddef.h>
#include <stdint.h>

#define CACHE_SIZE 256

typedef struct {
  union {
    size_t length;
    struct {
      uint64_t free : 1;
      uint64_t cached : 1;
      uint64_t ignored : 1;
      uint64_t length_over_8 : 61;
    };
  };
} heap_data_t;

#endif
