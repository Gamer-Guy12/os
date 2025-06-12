#ifndef KHEAP_H
#define KHEAP_H

#include <mem/memory.h>
#include <stddef.h>
#include <stdint.h>

typedef struct heap_entry_struct {
  union {
    struct heap_entry_struct *next;
    struct {
      uint64_t free : 1;
      uint64_t ignored : 2;
      uint64_t dont_use : 61;
    };
  };
} heap_entry_t;

/// the root one is stored at the start_brk
typedef struct heap_cache_struct {
  size_t object_size;
  heap_entry_t *first_entry;

  struct heap_cache_struct *parent;
  struct heap_cache_struct *right;
  struct heap_cache_struct *left;
} heap_cache_t;

/// Literally just align it to eight bytes
/// It also reserves the space for the root_cache
void init_heap(void);

heap_cache_t *create_heap_cache(size_t size);
heap_cache_t *find_heap_cache(size_t size);

// Try to find the cache but if it can't be found, create a heap
heap_cache_t *get_heap_cache(size_t size);

// Tries to find a free one but if it can't find one then it will create a new
// one
heap_entry_t *get_entry_in_cache(heap_cache_t *cache, size_t size);
void return_entry_to_cache(heap_cache_t *cache, heap_entry_t *entry);

heap_entry_t *create_entry_in_cache(heap_cache_t *cache, size_t size);
heap_entry_t *find_entry_in_cache(heap_cache_t *cache, size_t size);

#endif

