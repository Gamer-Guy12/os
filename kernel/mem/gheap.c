#include "kernel/gheap.h"
#include "kernel/kprintf.h"
#include "kernel/mem.h"
#include "lib/freelist.h"
#include "lib/list.h"
#include "lib/rbtree.h"
#include "lib/spinlock.h"
#include "util.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// This means that when there is less than or equal to 75% of the slab being
// used, try putting the slab descriptor within it
#define USAGE_THRESH 750

static GHEAP_CACHE(slab_cache);

static int sort_addresses(struct rbnode *n1, struct rbnode *n2) {
  struct gheap_slab *slab1 =
      (struct gheap_slab *)((uintptr_t)n1 -
                            offsetof(struct gheap_slab, tree_node));
  struct gheap_slab *slab2 =
      (struct gheap_slab *)((uintptr_t)n2 -
                            offsetof(struct gheap_slab, tree_node));

  uintptr_t addr = (uintptr_t)slab1->addr;
  uintptr_t target_addr = (uintptr_t)slab2->addr;
  size_t size = (1 << slab1->cache->alloc_order) * PAGE_SIZE;

  if (addr <= target_addr && addr + size > target_addr) {
    return 0;
  }

  if (addr > target_addr) {
    return 1;
  }

  return -1;
}

void *gmalloc(size_t size);
void gfree(void *ptr);

static bool calculate_sizes(size_t object_size, int *order,
                            uint32_t *objects_per_slab, bool slab_in) {
  int cur_order = -1;
  uint64_t cur_usage = 0;
  uint32_t cur_count = 0;
  bool do_slab_in = false;

  if (slab_in)
    goto slab_in;

  for (int i = 0; i < MAX_ORDER; i++) {
    const size_t size = (1 << i) * PAGE_SIZE;
    if (size < object_size)
      continue;

    const size_t count = size / object_size;
    const size_t usage_size = object_size * count;
    const size_t usage = usage_size * 1000 / size;

    if (usage > cur_usage) {
      cur_usage = usage;
      cur_count = count;
      cur_order = i;
    }
  }

  if (cur_usage > USAGE_THRESH && cur_order != -1) {
    *order = cur_order;
    *objects_per_slab = cur_count;
    return do_slab_in;
  }

slab_in:
  for (int i = 0; i < MAX_ORDER; i++) {
    const size_t size = (1 << i) * PAGE_SIZE - sizeof(struct gheap_slab);
    if (size < object_size)
      continue;

    const size_t count = size / object_size;
    const size_t usage_size = object_size * count;
    const size_t usage = usage_size * 1000 / size;

    if (usage > cur_usage) {
      cur_usage = usage;
      cur_count = count;
      cur_order = i;
      do_slab_in = true;
    }
  }

  *order = cur_order;
  *objects_per_slab = cur_count;
  return do_slab_in;
}

// slab_in when true forces the slab descriptor to be inside the slab itself
// otherwise it can be either one
static void __gheap_cache_create(struct gheap_cache *cache, size_t object_size,
                                 int flags, bool slab_in) {
  spinlock_acquire(&cache->lock);

  // Already initialized
  if (cache->object_size != 0) {
    spinlock_release(&cache->lock);
    return;
  }

  rb_create(&cache->full_list, sort_addresses);
  rb_create(&cache->partial_list, sort_addresses);
  LIST_INIT(&cache->empty_list);

  cache->object_size = object_size;
  cache->flags = flags;
  cache->slab_in = calculate_sizes(object_size, &cache->alloc_order,
                                   &cache->objects_per_slab, slab_in);

  if (cache->alloc_order == -1) {
    kprintf("Failed to calculate an allocation order for slab with object "
            "size: 0x%x\n",
            object_size);
    panic();
  }

  spinlock_release(&cache->lock);
}

// Flags are going to be the flags used to allocate and free pages
void gheap_cache_create(struct gheap_cache *cache, size_t object_size,
                        int flags) {
  if (object_size < GHEAP_MIN_SIZE)
    object_size = GHEAP_MIN_SIZE;

  // Later on make it so that this can't fail
  if (slab_cache.object_size == 0)
    __gheap_cache_create(&slab_cache, sizeof(struct gheap_slab), ZONE_ANY,
                         true);

  // If the flags don't allow the allocation to fail you should probably set the
  // slab_in to be true but that flag doesn't exist yet so idgaf
  __gheap_cache_create(&slab_cache, object_size, flags, false);
}

// Will remove the slab from its list
static struct gheap_slab *__gheap_slab_get(struct gheap_cache *cache) {
  // First check the partial list
  struct rbnode *partial_node = rb_delete_min(&cache->partial_list, NULL);
  if (partial_node) {
    return (struct gheap_slab *)((uintptr_t)partial_node -
                                 offsetof(struct gheap_slab, tree_node));
  }

  if (LIST_EMPTY(&cache->empty_list)) {
    return NULL;
  }

  struct list_node *empty_node = cache->empty_list.next;
  list_remove(empty_node);

  return (struct gheap_slab *)((uintptr_t)empty_node -
                               offsetof(struct gheap_slab, list_node));
}

static struct gheap_slab *__gheap_slab_create(struct gheap_cache *cache) {
  struct gheap_slab *slab = NULL;
  uintptr_t addr = 0;

  if (!cache->slab_in) {
    // Currently this can fail but in the future it won't be able to but well
    // still pretend it can
    slab = gheap_cache_alloc(&slab_cache);
    if (!slab) {
      return NULL;
    }

    slab->addr = alloc_pages(cache->alloc_order, cache->flags);
    if (!slab->addr) {
      gheap_cache_free(&slab_cache, slab);
      return NULL;
    }

    addr = (uintptr_t)slab->addr;
  } else {
    addr = (uintptr_t)alloc_pages(cache->alloc_order, cache->flags);
    if (!addr) {
      return NULL;
    }

    slab = (struct gheap_slab *)addr;
    addr += sizeof(struct gheap_slab);
  }

  slab->cache = cache;
  FREELIST_INIT(&slab->freelist);
  slab->count_left = cache->objects_per_slab;

  // Create freelist
  for (size_t i = 0; i < slab->count_left; i++) {
    freelist_insert(&slab->freelist, (void *)addr);

    addr += cache->object_size;
  }

  return slab;
}

void *gheap_cache_alloc(struct gheap_cache *cache) {
  spinlock_acquire(&cache->lock);
  // Check if uninitialized
  if (cache->object_size == 0) {
    spinlock_release(&cache->lock);
    return NULL;
  }

  // Get slab
  struct gheap_slab *slab = NULL;
  for (int i = 0; i < GHEAP_RETRY_COUNT; i++) {
    slab = __gheap_slab_get(cache);
    if (slab)
      break;
  }

  if (!slab)
    slab = __gheap_slab_create(cache);
  if (!slab) {
    spinlock_release(&cache->lock);
    return NULL;
  }

  // Allocate within it
  void *addr = freelist_remove(&slab->freelist);
  slab->count_left--;

  // Add it back to the list
  // If the slab doesn't have any more objects then put it in the full list
  // otherwise in the partial list
  rb_insert(slab->count_left == 0 ? &cache->full_list : &cache->partial_list,
            &slab->tree_node);
  spinlock_release(&cache->lock);

  return addr;
}

// Zone does not to be included
void gheap_cache_free(struct gheap_cache *cache, void *ptr) {
  if (!ptr)
    return;

  spinlock_acquire(&cache->lock);
  // Get slab
  struct gheap_slab dummy = {.addr = ptr};
  struct rbnode *node =
      rb_delete_search(&cache->full_list, NULL, &dummy.tree_node);
  if (!node)
    node = rb_delete_search(&cache->partial_list, NULL, &dummy.tree_node);
  if (!node) {
    spinlock_release(&cache->lock);
    return;
  }

  struct gheap_slab *slab =
      (struct gheap_slab *)((uintptr_t)node -
                            offsetof(struct gheap_slab, tree_node));

  freelist_insert(&slab->freelist, ptr);
  slab->count_left--;

  // Reinsert into the cache
  if (slab->count_left == cache->objects_per_slab) {
    list_insert(&cache->empty_list, &slab->list_node);
  } else {
    rb_insert(&cache->partial_list, &slab->tree_node);
  }

  spinlock_release(&cache->lock);
}

static void __slab_destroy(struct gheap_cache *cache, struct gheap_slab *slab) {
  free_pages(slab->addr, cache->alloc_order);

  if (!cache->slab_in)
    gheap_cache_free(&slab_cache, slab);
}

// This frees all slabs (even if there is stuff allocated within)
void gheap_cache_destroy(struct gheap_cache *cache) {
  while (true) {
    struct rbnode *node = rb_delete_max(&cache->full_list, NULL);
    if (!node)
      break;
    struct gheap_slab *slab =
        (struct gheap_slab *)((uintptr_t)node -
                              offsetof(struct gheap_slab, tree_node));

    __slab_destroy(cache, slab);
  }

  while (true) {
    struct rbnode *node = rb_delete_max(&cache->partial_list, NULL);
    if (!node)
      break;
    struct gheap_slab *slab =
        (struct gheap_slab *)((uintptr_t)node -
                              offsetof(struct gheap_slab, tree_node));

    __slab_destroy(cache, slab);
  }

  while (!LIST_EMPTY(&cache->empty_list)) {
    struct list_node *node = cache->empty_list.next;
    struct gheap_slab *slab =
        (struct gheap_slab *)((uintptr_t)node -
                              offsetof(struct gheap_slab, list_node));

    __slab_destroy(cache, slab);
  }
}
