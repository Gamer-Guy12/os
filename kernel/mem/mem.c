#include "kernel/mem.h"
#include <stddef.h>
#include <stdint.h>

struct page *__get_page_struct(pageptr_t ptr, int zone) {
  // Zone struct
  const struct zone *s_zone = __get_zone(zone);
  const uintptr_t base = (uintptr_t)s_zone->base;
  const uintptr_t index = base / PAGE_SIZE + ptr;

  return &pages[index];
}

pageptr_t __get_page_pointer(struct page *page, int zone) {
  const uintptr_t addr = (uintptr_t)page;
  size_t global_index = (addr - PAGE_STRUCT_OFFSET) / sizeof(struct page);
  return global_index - (uintptr_t)__get_zone(zone)->base / PAGE_SIZE;
}

struct page *__paddr_page_struct(void *addr) {
  uintptr_t addr_bits = (uintptr_t)addr;
  size_t page_count = addr_bits / PAGE_SIZE;
  return &pages[page_count];
}
