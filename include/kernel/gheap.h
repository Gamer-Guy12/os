#ifndef _KERNEL_GHEAP_H_
#define _KERNEL_GHEAP_H_

#include "lib/freelist.h"
#include "lib/list.h"
#include "lib/rbtree.h"
#include "lib/spinlock.h"
#include <stddef.h>
#include <stdint.h>

#define GHEAP_RETRY_COUNT 3
#define GHEAP_MIN_SIZE 32

// This is 64 bytes (at least rn) :)
struct gheap_slab {
  union {
    struct list_node list_node;
    struct rbnode tree_node;
  };
  struct freelist_node freelist;
  struct gheap_cache *cache;
  void *addr;
  size_t count_left;
};

struct gheap_cache {
  // When searching for a slab that contains an address set slab->addr to be the
  // address that is within the slab
  struct rbtree full_list;
  struct rbtree partial_list;
  // This is only a list because we don't need to search pages within here
  struct list_node empty_list;
  // When 0 this means the cache is uninitialized
  size_t object_size;
  // How many are unallocated
  uint32_t object_count;
  uint32_t objects_per_slab;
  int alloc_order;
  int flags;
  spinlock_t lock;
  // Should slab be kept in the slab cache or as part of the slab
  bool slab_in;
};

// Allocates from ZONE_ANY
// Allocates with blocking and waiting and while it can't panic it can return
// null and all that jazz
//
// If you need that control use the page allocator or use gheap or make it
// urself (im essentially pushing this work to later, but i think its a good
// idea to do so)
void *gmalloc(size_t size);
void gfree(void *ptr);

// Flags are going to be the flags used to allocate and free pages
void gheap_cache_create(struct gheap_cache *cache, size_t object_size,
                        int flags);
void *gheap_cache_alloc(struct gheap_cache *cache);
// Zone does not to be included
void gheap_cache_free(struct gheap_cache *cache, void *ptr);
void gheap_cache_destroy(struct gheap_cache *cache);

#define GHEAP_CACHE(name)                                                      \
  struct gheap_cache name = {                                                  \
      .object_size = 0, .lock = SPINLOCK_ZERO(name##_lock), .object_count = 0}

#endif
