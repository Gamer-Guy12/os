#include "kernel/mem.h"
#include "util.h"

struct gheap_freelist {
  page_ptr_t freelist;
  uint32_t alloc_order;
};

#ifdef _x86_64_
#define MIN_SIZE 8
#define ORDER_COUNT 19
#else
#error "Cannot calculate gheap data"
#endif

struct gheap_freelist freelists[ORDER_COUNT * ALLOC_COUNT];

INIT void init_gheap(void) {

}

