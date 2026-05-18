#include "kernel/gheap.h"
#include "kernel/kprintf.h"
#include "kernel/mem.h"
#include "lib/atomic.h"
#include "util.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static struct gheap_cache slab_cache;

#define MIN_OBJECT_SIZE 32
#define RETRY_COUNT 3

static size_t cache_sizes[] = {
#ifdef _x86_64_
#define DEFAULT_CACHE_COUNT 19

    32,   64,    96,    128,   192,    256,    512,    1024,    2048,    4096,
    8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576, 0x200000
#else
#error "Cannot calculate Gheap cache sizes"
#endif
};

// If there are 3 zones then index 0 is size 0 and index 3 is size 3
static struct gheap_cache default_caches[DEFAULT_CACHE_COUNT * ZONE_COUNT];

static void init_cache(struct gheap_cache *cache, size_t object_size,
                       uint32_t zone, bool seperate_slab) {
  cache->object_size = object_size;
  cache->zone = zone;

  size_t best_percent = 0;
  size_t best_order = 0;
  size_t best_object_count = 0;

  for (uint32_t i = 0; i < MAX_ORDER; i++) {
    const size_t page_sizes = PAGE_SIZE * (1 << i);
    const size_t object_count =
        (page_sizes - sizeof(struct gheap_slab)) / object_size;
    const size_t usage_percent = object_count * object_size * 1000 / page_sizes;

    if (best_percent < usage_percent) {
      best_percent = usage_percent;
      best_order = i;
      best_object_count = object_count;
    }
  }

  if (!seperate_slab) {
    goto size_config;
  }

  seperate_slab = false;

  for (uint32_t i = 0; i < MAX_ORDER; i++) {
    const size_t page_sizes = PAGE_SIZE * (1 << i);
    const size_t object_count = (page_sizes) / object_size;
    const size_t usage_percent = object_count * object_size * 1000 / page_sizes;

    if (best_percent < usage_percent) {
      best_percent = usage_percent;
      best_order = i;
      best_object_count = object_count;
      seperate_slab = true;
    }
  }

size_config:
  cache->object_count = best_object_count;
  cache->alloc_order = best_order;
  cache->seperate_slab = seperate_slab;

  LIST_INIT(&cache->empty_list);
  LIST_INIT(&cache->partial_list);
  LIST_INIT(&cache->full_list);
}

void gheap_cache_create(struct gheap_cache *cache, size_t object_size,
                        uint32_t zone) {
  init_cache(cache, object_size, zone, true);
}

void INIT init_gheap(void) {
  init_cache(&slab_cache, sizeof(struct gheap_slab), ZONE_ANY, false);

  for (int i = 0; i < ZONE_COUNT; i++) {
    for (int j = 0; j < DEFAULT_CACHE_COUNT; j++) {
      init_cache(&default_caches[j * ZONE_COUNT + i], cache_sizes[j], i, true);
    }
  }

  // kprintf("[MEM] Initialized Gheap\n");
}

static void *slab_create(struct gheap_slab *slab, struct gheap_cache *cache,
                         size_t object_size, uint32_t zone,
                         bool seperate_slab) {
  if (object_size < MIN_OBJECT_SIZE) {
    object_size = MIN_OBJECT_SIZE;
  }

  void *start_addr = alloc_pages(cache->alloc_order, zone);
  if (start_addr == NULL)
    return NULL;
  if (!seperate_slab)
    slab = start_addr;

  slab->cache = cache;
  slab->start_addr = start_addr;
  atomic_store(&slab->count_left, slab->cache->object_count);

  FREELIST_INIT(&slab->freelist);

  // Mark which slab each page is in
  page_ptr_t base_ptr =
      ((uintptr_t)start_addr - IDENTITY_MAP_OFFSET) / PAGE_SIZE;

  for (size_t i = 0; i < (1 << cache->alloc_order); i++) {
    get_page(base_ptr + i)->slab = slab;
  }

  for (uint32_t i = 0; i < slab->cache->object_count; i++) {
    freelist_insert(&slab->freelist,
                    (void *)((uintptr_t)slab->start_addr + i * object_size +
                             (seperate_slab ? 0 : sizeof(struct gheap_slab))));
  }

  return slab;
}

static void slab_destroy(struct gheap_slab *slab) {
  free_pages(slab->start_addr, slab->cache->alloc_order);
}

static void *slab_alloc(struct gheap_slab *slab, int *count) {
  // Obtain from the freelist
  int old_count = atomic_load(&slab->count_left);

  while (true) {
    if (old_count <= 0) {
      return NULL;
    }

    if (atomic_cas(&slab->count_left, old_count, old_count - 1)) {
      break;
    }
  }

  *count = old_count - 1;

  return freelist_get(&slab->freelist);
}

void slab_free(struct gheap_slab *slab, void *ptr) {
  freelist_insert(&slab->freelist, ptr);
  atomic_add(&slab->count_left, 1);
}

// Find a single slab and attempt to allocate from it
// I'm not retrying while holding the lock so that slabs can be moved if
// necessary without having to wait through locks and burn time for both
// allocators and the person tryna move it
static void *try_cache_alloc(struct gheap_cache *cache) {
  struct gheap_slab *slab = NULL;

  rw_read_acquire(&cache->lock);

  if (!LIST_EMPTY(&cache->partial_list)) {
    struct list_node *node = cache->partial_list.next;
    slab = (void *)((uintptr_t)node - offsetof(struct gheap_slab, node));
  } else {
    // Try to move an empty slab into partial
    if (LIST_EMPTY(&cache->empty_list)) {
      rw_read_release(&cache->lock);
      return NULL;
    }
    rw_read_release(&cache->lock);
    rw_write_acquire(&cache->lock);
    struct list_node *node = cache->empty_list.next;
    slab = (void *)((uintptr_t)node - offsetof(struct gheap_slab, node));
    list_remove(node);
    list_insert(&cache->partial_list, node);
    rw_write_release(&cache->lock);
  }

  if (slab == NULL) {
    rw_read_release(&cache->lock);
    return NULL;
  }

  int count = 0;
  void *ptr = slab_alloc(slab, &count);

  if (ptr == NULL || count != 0) {
    rw_read_release(&cache->lock);
    return ptr;
  }

  rw_read_release(&cache->lock);

  rw_write_acquire(&cache->lock);

  // Re reading to make sure that in between allocating from the slab and here
  // something wasn't freed
  //
  // Since currently there is a lock being held, from here till the lock's
  // release the count is valid
  count = atomic_load(&slab->count_left);
  if (count == 0) {
    struct list_node *node = &slab->node;
    list_remove(node);
    list_insert(&cache->full_list, node);
  }

  rw_write_release(&cache->lock);

  return ptr;
}

void *gheap_cache_alloc(struct gheap_cache *cache) {
  void *ret = NULL;

  for (int i = 0; i < RETRY_COUNT; i++) {
    ret = try_cache_alloc(cache);
    if (ret)
      return ret;
  }

  // Make a slab to use
  struct gheap_slab *slab = NULL;

  if (cache->seperate_slab) {
    slab = gheap_cache_alloc(&slab_cache);
    slab_create(slab, cache, cache->object_size, cache->zone, true);
  } else {
    slab = slab_create(NULL, cache, cache->object_size, cache->zone, false);
  }

  if (slab == NULL)
    return NULL;

  int count = 0;
  // Should always succeed
  ret = slab_alloc(slab, &count);

  rw_write_acquire(&cache->lock);
  if (count == 0) {
    list_insert(&cache->full_list, &slab->node);
  } else {
    list_insert(&cache->partial_list, &slab->node);
  }
  rw_write_release(&cache->lock);

  return ret;
}

void gheap_cache_free(struct gheap_cache *cache, void *ptr) {
  struct page *page = addr_page(ptr);
  struct gheap_slab *slab = page->slab;

  rw_read_acquire(&cache->lock);
  slab_free(slab, ptr);
  rw_read_release(&cache->lock);

  rw_write_acquire(&cache->lock);
  int count = atomic_load(&slab->count_left);
  if (count != 1 && count != cache->object_count) {
    rw_write_release(&cache->lock);
    return;
  }

  list_remove(&slab->node);
  if (count == 1) {
    list_insert(&cache->partial_list, &slab->node);
  } else if (count == cache->object_count) {
    list_insert(&cache->empty_list, &slab->node);
  }
  rw_write_release(&cache->lock);
}

void gheap_cache_destroy(struct gheap_cache *cache) {
  while (cache->empty_list.next != &cache->empty_list) {
    struct list_node *node = cache->empty_list.next;
    struct gheap_slab *slab =
        (void *)((uintptr_t)node - offsetof(struct gheap_slab, node));

    list_remove(node);
    slab_destroy(slab);

    if (cache->seperate_slab)
      gheap_cache_free(&slab_cache, slab);
  }

  while (cache->partial_list.next != &cache->partial_list) {
    struct list_node *node = cache->partial_list.next;
    struct gheap_slab *slab =
        (void *)((uintptr_t)node - offsetof(struct gheap_slab, node));

    list_remove(node);
    slab_destroy(slab);

    if (cache->seperate_slab)
      gheap_cache_free(&slab_cache, slab);
  }

  while (cache->full_list.next != &cache->full_list) {
    struct list_node *node = cache->full_list.next;
    struct gheap_slab *slab =
        (void *)((uintptr_t)node - offsetof(struct gheap_slab, node));

    list_remove(node);
    slab_destroy(slab);

    if (cache->seperate_slab)
      gheap_cache_free(&slab_cache, slab);
  }
}

void gfree(void *ptr) {
  RMEMB();
  struct page *page = addr_page(ptr);
  struct gheap_slab *slab = page->slab;
  struct gheap_cache *cache = slab->cache;

  gheap_cache_free(cache, ptr);
}

void *gmalloc(size_t size, uint32_t zone) {
  uint32_t alloc_index = 0;

  for (int i = DEFAULT_CACHE_COUNT - 1; i >= 0; i--) {
    if (size <= cache_sizes[i]) {
      alloc_index = i;
    }
  }

  return gheap_cache_alloc(&default_caches[alloc_index * ZONE_COUNT + zone]);
}
