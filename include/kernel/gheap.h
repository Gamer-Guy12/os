#ifndef _KERNEL_GHEAP_H_
#define _KERNEL_GHEAP_H_

#include "lib/freelist.h"
#include "lib/list.h"
#include "lib/rw_lock.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct gheap_slab {
  struct list_node node;
  struct freelist_node freelist;
  struct gheap_cache* cache;
  void *start_addr;
  atomic_t count_left;
};

// Read acquire to work with the slabs and write acquire to move slabs or delete
// them
struct gheap_cache {
  struct list_node full_list;
  struct list_node partial_list;
  struct list_node empty_list;
  size_t object_size;
  uint32_t zone;
  rw_lock_t lock;
  bool seperate_slab;
  uint32_t alloc_order;
  uint32_t object_count;
};

// Only supports allocating for the kernel
void *gmalloc(size_t size, uint32_t zone);
void gfree(void *ptr);
void init_gheap(void);

void *gheap_cache_alloc(struct gheap_cache* cache);
// Less convenient than gfree but slightly faster
void gheap_cache_free(struct gheap_cache* cache, void *ptr);
void gheap_cache_create(struct gheap_cache* cache, size_t object_size, uint32_t zone);
void gheap_cache_destroy(struct gheap_cache* cache);

#endif
