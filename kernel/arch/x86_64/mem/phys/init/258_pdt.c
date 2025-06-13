#include <mem/pimemory.h>
#include <stddef.h>

size_t used_count = 0;

size_t reserve_258_pdt_page(size_t count) {
  used_count += count;
  return used_count - count;
}

size_t get_used_258_pdt_page_count(void) { return used_count; }
