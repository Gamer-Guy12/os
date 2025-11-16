#include "kernel/mem.h"
#include <stddef.h>
#include <stdint.h>

struct page *get_page(page_ptr_t ptr) { return &pages[ptr]; }

void *alloc_pages(uint32_t order, uint32_t flags) {
  uint32_t zone = GET_ZONE(flags);

  if (zone == ZONE_NULL) {
    return NULL;
  }

  void *ptr = __alloc_pages(order, zone);
  page_ptr_t page_index = (uintptr_t)ptr / PAGE_SIZE;

  struct page *page = get_page(page_index);

  page->flags = GET_PAGE_FLAGS(flags);

  return (void *)((uintptr_t)ptr + IDENTITY_MAP_OFFSET);
}

void free_pages(void *addr, uint32_t order) {
  uintptr_t actual_addr = (uintptr_t)addr - IDENTITY_MAP_OFFSET;

  __free_pages((void *)actual_addr, order);
}

struct page* addr_page(void *addr) {
  page_ptr_t page_index = ((uintptr_t)addr - IDENTITY_MAP_OFFSET) / PAGE_SIZE;
  return get_page(page_index);
}
