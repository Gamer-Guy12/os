#ifndef _KERNEL_GHEAP_H_
#define _KERNEL_GHEAP_H_

#include "lib/freelist.h"
#include "lib/list.h"
#include "lib/spinlock.h"
#include <stddef.h>
#include <stdint.h>

#define GHEAP_RETRY_COUNT 3
#define GHEAP_MIN_SIZE 32
// This means that when there is less than or equal to 75% of the slab being
// used, try putting the slab descriptor within it
#define GHEAP_USAGE_THRESHOLD 750

__attribute__((unused)) static size_t gheap_cache_sizes[] = {
#ifdef _x86_64_
#define GHEAP_CACHE_COUNT 19
    0x20,    0x40,    0x60,    0x80,     0xC0,    0x100,  0x200,
    0x400,   0x800,   0x1000,  0x2000,   0x4000,  0x8000, 0x10000,
    0x20000, 0x40000, 0x80000, 0x100000, 0x200000
#else
#error "Cannot calculate gheap default cache sizes"
#endif
};

// This is 48 bytes (at least rn) :| (its no longer 64)
struct gheap_slab {
  struct list_node node;
  struct freelist_node freelist;
  struct gheap_cache *cache;
  void *addr;
  size_t count_left;
};

struct gheap_cache {
  // When searching for a slab that contains an address set slab->addr to be the
  // address that is within the slab
  struct list_node full_list;
  struct list_node partial_list;
  // This is only a list because we don't need to search pages within here
  struct list_node empty_list;
  // When 0 this means the cache is uninitialized
  size_t object_size;
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
  struct gheap_cache name = {.object_size = 0,                                 \
                             .lock = SPINLOCK_ZERO(name##_lock)}

#endif
