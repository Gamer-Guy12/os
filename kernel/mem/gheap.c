#include "kernel/gheap.h"
#include "kernel/kprintf.h"
#include "kernel/mem.h"
#include "lib/atomic.h"
#include "lib/freelist.h"
#include "lib/list.h"
#include "lib/spinlock.h"
#include "util.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static GHEAP_CACHE(slab_cache);
// Since these aren't initialized with GHEAP_CACHE, you have to manually clear
// them with like SPINLOCK_ZERO
static atomic_t initialized = ATOMIC_ZERO;
// Allocates only from zone normal
static struct gheap_cache default_caches[GHEAP_CACHE_COUNT] = {0};

// This is a bit slower than the caches and less granular
void *gmalloc(size_t size) {
  int cache = -1;
  for (int i = GHEAP_CACHE_COUNT - 1; i >= 0; i--) {
    if (size <= gheap_cache_sizes[i])
      cache = i;
    else
      break;
  }

  if (cache == -1)
    return NULL;

  // Check if initialized
  if (atomic_cas(&initialized, 0, 1)) {
    for (int i = 0; i < GHEAP_CACHE_COUNT; i++) {
      default_caches[i].object_size = 0;
      default_caches[i].lock = (spinlock_t)SPINLOCK_ZERO(gheap_cache_lock);
      gheap_cache_create(&default_caches[i], gheap_cache_sizes[i], ZONE_ANY);
    }
    atomic_store(&initialized, 2);
  } else {
    while (atomic_load(&initialized) != 2)
      ;
  }

  // Allocate
  struct gheap_cache *cache_desc = &default_caches[cache];
  return gheap_cache_alloc(cache_desc);
}

void gfree(void *ptr) {
  // Get the page
  struct page *page = __paddr_page_struct(VTP(ptr));
  struct gheap_slab *slab = page->slab;
  struct gheap_cache *cache = slab->cache;

  gheap_cache_free(cache, ptr);
}

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

  if (cur_usage > GHEAP_USAGE_THRESHOLD && cur_order != -1) {
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

  LIST_INIT(&cache->full_list);
  LIST_INIT(&cache->partial_list);
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
  __gheap_cache_create(cache, object_size, flags, false);
}

// Will remove the slab from its list
static struct gheap_slab *__gheap_slab_get(struct gheap_cache *cache) {
  // First check the partial list
  if (!LIST_EMPTY(&cache->partial_list)) {
    struct list_node *partial_node = cache->partial_list.next;
    list_remove(partial_node);

    return (struct gheap_slab *)((uintptr_t)partial_node -
                                 offsetof(struct gheap_slab, node));
  }

  if (LIST_EMPTY(&cache->empty_list)) {
    return NULL;
  }

  struct list_node *empty_node = cache->empty_list.next;
  list_remove(empty_node);

  return (struct gheap_slab *)((uintptr_t)empty_node -
                               offsetof(struct gheap_slab, node));
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
    if (addr == 0) {
      return NULL;
    }

    slab = (struct gheap_slab *)addr;
    slab->addr = (void *)addr;
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

  // Tell the pages what the slab is
  struct page *page = __paddr_page_struct(VTP(slab->addr));
  for (int i = 0; i < (1 << cache->alloc_order); i++) {
    page[i].slab = slab;
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

  if (!slab) {
    slab = __gheap_slab_create(cache);
  }
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
  list_insert(slab->count_left == 0 ? &cache->full_list : &cache->partial_list,
              &slab->node);
  spinlock_release(&cache->lock);

  return addr;
}

// Zone does not to be included
void gheap_cache_free(struct gheap_cache *cache, void *ptr) {
  if (!ptr)
    return;

  spinlock_acquire(&cache->lock);
  // Get slab
  struct page *page = __paddr_page_struct(VTP(ptr));
  struct gheap_slab *slab = page->slab;

  freelist_insert(&slab->freelist, ptr);
  slab->count_left--;

  // Reinsert into the cache
  list_insert(slab->count_left == cache->objects_per_slab
                  ? &cache->empty_list
                  : &cache->partial_list,
              &slab->node);

  spinlock_release(&cache->lock);
}

static void __slab_destroy(struct gheap_cache *cache, struct gheap_slab *slab) {
  free_pages(slab->addr, cache->alloc_order);

  if (!cache->slab_in)
    gheap_cache_free(&slab_cache, slab);
}

// This frees all slabs (even if there is stuff allocated within)
void gheap_cache_destroy(struct gheap_cache *cache) {
  while (!LIST_EMPTY(&cache->full_list)) {
    struct list_node *node = cache->full_list.next;
    struct gheap_slab *slab =
        (struct gheap_slab *)((uintptr_t)node -
                              offsetof(struct gheap_slab, node));

    __slab_destroy(cache, slab);
  }

  while (!LIST_EMPTY(&cache->partial_list)) {
    struct list_node *node = cache->partial_list.next;
    struct gheap_slab *slab =
        (struct gheap_slab *)((uintptr_t)node -
                              offsetof(struct gheap_slab, node));

    __slab_destroy(cache, slab);
  }

  while (!LIST_EMPTY(&cache->empty_list)) {
    struct list_node *node = cache->empty_list.next;
    struct gheap_slab *slab =
        (struct gheap_slab *)((uintptr_t)node -
                              offsetof(struct gheap_slab, node));

    __slab_destroy(cache, slab);
  }
}
