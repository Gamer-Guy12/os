#include "kernel/mem.h"
#include "lib/freelist.h"
#include "lib/list.h"

struct slab {
  page_ptr_t first_page;
  size_t object_size;
  struct freelist_node freelist;
  struct list_node node;
  uint32_t allocated_order;
  uint32_t allocated_flags;
};

#define GHEAP_MIN_SIZE 8
#define GHEAP_MAX_ORDER 19

struct list_node full_slabs[GHEAP_MAX_ORDER * ALLOC_COUNT];
struct list_node partial_slabs[GHEAP_MAX_ORDER * ALLOC_COUNT];

void init_gheap(void) {
  for (int i = 0; i < GHEAP_MAX_ORDER * ALLOC_COUNT; i++) {
  }
}

void *gmalloc(size_t size, uint32_t type);

void gfree(void *ptr);
