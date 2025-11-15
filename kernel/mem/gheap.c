#include "kernel/gheap.h"
#include "kernel/kprintf.h"
#include "kernel/mem.h"
#include "lib/rw_lock.h"
#include "util.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static struct gheap_cache cache_cache;
static struct gheap_cache slab_cache;

#define WASTE_THRESHOLD 850

static size_t cache_sizes[] = {
#ifdef _x86_64_
#define DEFAULT_CACHE_COUNT 19

    32,   64,    96,    128,   192,    256,    512,    1024,    2048,    4096,
    8192, 16384, 32768, 65536, 131072, 262144, 524288, 1048576, 0x200000
#else
#error "Cannot calculate Gheap cache sizes"
#endif
};

static struct gheap_cache default_caches[DEFAULT_CACHE_COUNT * ZONE_COUNT];

static uint32_t calculate_order(size_t object_size, bool *seperate_slab) {
  size_t best_percent = 0;
  size_t best_order = 0;
  bool is_seperate_slab = false;

  for (uint32_t i = 0; i < MAX_ORDER; i++) {
    const size_t page_sizes = PAGE_SIZE * (1 << i);
    const size_t object_count =
        (page_sizes - sizeof(struct gheap_slab)) / object_size;
    const size_t usage_percent =
        (object_count * object_size * 1000) / page_sizes;

    if (best_percent < usage_percent) {
      best_percent = usage_percent;
      best_order = i;
    }
  }

  if (!*seperate_slab || best_percent > WASTE_THRESHOLD) {
    *seperate_slab = false;
    return best_order;
  }

  for (uint32_t i = 0; i < MAX_ORDER; i++) {
    const size_t page_sizes = PAGE_SIZE * (1 << i);
    const size_t object_count = page_sizes / object_size;
    const size_t usage_percent = object_count * object_size * 1000 / page_sizes;

    if (best_percent < usage_percent) {
      best_percent = usage_percent;
      best_order = i;
      is_seperate_slab = true;
    }
  }

  *seperate_slab = is_seperate_slab;
  return best_order;
}

static void init_cache(struct gheap_cache *cache, size_t object_size,
                       uint32_t zone, bool seperate_slab) {
  cache->object_size = object_size;
  cache->zone = zone;

  LIST_INIT(&cache->empty_list);
  LIST_INIT(&cache->partial_list);
  LIST_INIT(&cache->full_list);

  cache->seperate_slab = seperate_slab;
  cache->alloc_order = calculate_order(object_size, &cache->seperate_slab);
}

void INIT init_gheap(void) {
  kprintf("\t[MEM] Initializing Gheap\n");

  init_cache(&cache_cache, sizeof(struct gheap_cache), ZONE_ANY, false);
  init_cache(&slab_cache, sizeof(struct gheap_slab), ZONE_ANY, false);

  for (int i = 0; i < ZONE_COUNT; i++) {
    for (int j = 0; j < DEFAULT_CACHE_COUNT; j++) {
      init_cache(&default_caches[j * ZONE_COUNT + i], cache_sizes[j], i, true);
    }
  }

  kprintf("\t[MEM] Initialized Gheap\n");
}
