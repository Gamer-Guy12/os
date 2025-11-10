#ifndef _KERNEL_GHEAP_H_
#define _KERNEL_GHEAP_H_

#include "kernel/mem.h"
#include "lib/atomic.h"
#include "lib/freelist.h"
#include "lib/list.h"

#define GHEAP_MIN_SIZE 32
#define GHEAP_MAX_ORDER 17

struct slab {
  struct list_node node;
  void *start_addr;
  struct freelist_node freelist;
  struct cache *cache;
  atomic_t count_left;
};

struct cache {
  struct list_node full_slabs;
  struct list_node partial_slabs;
  struct list_node empty_slabs;
  // The order of pages to allocate (pre caluclated at the start)
  uint32_t allocated_order;
  // The Alloc flags
  uint32_t allocated_flags;
  size_t object_size;
  spinlock_t lock;
};

void init_gheap(void);

void *gmalloc(size_t size, uint32_t type);
void gfree(void *ptr);

#endif
