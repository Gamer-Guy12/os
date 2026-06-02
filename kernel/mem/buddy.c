#include "kernel/mem.h"
#include "lib/bit.h"
#include "lib/spinlock.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static struct zone *get_zone(int flags) {
  int zone_portion = flags & ZONE_MASK;
  return __get_zone(zone_portion);
}

static void freelist_remove(struct page *page) {
  page->next->prev = page->prev;
  page->prev->next = page->next;

  page->prev = NULL;
  page->next = NULL;
}

static void free_in_layer(struct page *page, struct buddy_layer *layer,
                          pageptr_t ptr, int order) {
  page->next = layer->freelist.next;
  page->prev = &layer->freelist;

  page->next->prev = page;
  page->prev->next = page;

  size_t index = ptr / (1 << (order + 1));
  flip_bit_in_ptr(layer->data, index);
}

void *_alloc_pages(int order, int flags) {
  struct zone *zone = get_zone(flags);
  int desired_order = order;
  order = -1;
  struct page *page = NULL;
  pageptr_t ptr = 0;

  _spinlock_acquire(&zone->lock);

  // Get page from free list
  for (int i = desired_order; i < MAX_ORDER; i++) {
    struct buddy_layer *layer = &zone->buddy[i];
    if (layer->freelist.next != &layer->freelist) {
      page = layer->freelist.next;
      order = i;
      freelist_remove(page);

      // Get page out of bitmap
      ptr = __get_page_pointer(page, flags & ZONE_MASK);
      size_t index = ptr / (1 << (i + 1));
      flip_bit_in_ptr(layer->data, index);

      break;
    }
  }

  if (order == -1) {
    _spinlock_release(&zone->lock);
    return NULL;
  }

  // Split downwards
  //
  // It took me way to fucking long to releaize it should be:
  // int i = order - 1; i >= desired_order; i--
  // and not:
  // int i = order; i > desired_order; i--
  for (int i = order - 1; i >= desired_order; i--) {
    // Get partner and free it
    pageptr_t partner_ptr = ptr ^ (1 << i);
    struct page *partner = __get_page_struct(partner_ptr, flags & ZONE_MASK);

    free_in_layer(partner, &zone->buddy[i], partner_ptr, i);
  }

  _spinlock_release(&zone->lock);

  const size_t index =
      ((uintptr_t)page - PAGE_STRUCT_OFFSET) / sizeof(struct page);
  const uintptr_t paddr = index * PAGE_SIZE;

  return (void *)(paddr + IDENTITY_OFFSET);
}

void _free_pages(void *addr, int order, int flags) {
  if (addr == NULL) {
    return;
  }

  struct zone *zone = NULL;
  int zone_index = 0;
  addr = VTP(addr);

  for (int i = 0; i < ZONE_COUNT; i++) {
    struct zone *c_zone = __get_zone(i);
    if ((uintptr_t)addr >= (uintptr_t)c_zone->base &&
        (uintptr_t)addr < (uintptr_t)c_zone->base + c_zone->length) {
      zone = c_zone;
      zone_index = i;
      break;
    }
  }

  if (!zone) {
    return;
  }

  struct page *page = __paddr_page_struct(addr);
  pageptr_t ptr = __get_page_pointer(page, zone_index);

  _spinlock_acquire(&zone->lock);

  // At each level check if the partner is free, if so merge, if not keep going
  // Insert if you are at the top level (MAX_ORDER - 1)
  for (int i = order; i < MAX_ORDER; i++) {
    struct buddy_layer *layer = &zone->buddy[i];
    if (i == MAX_ORDER - 1) {
      free_in_layer(page, layer, ptr, i);
      break;
    }

    size_t index = ptr / (1 << (i + 1));
    if (check_bit_in_ptr(layer->data, index)) {
      // The partner is free
      pageptr_t partner_ptr = ptr ^ (1 << i);
      struct page *partner = __get_page_struct(partner_ptr, zone_index);

      // Allocate partner
      freelist_remove(partner);
      flip_bit_in_ptr(layer->data, index);

      bool older = partner_ptr < ptr;
      ptr = older ? partner_ptr : ptr;
      page = older ? partner : page;

      continue;
    }

    // The partner is allocated: put this into the layer
    free_in_layer(page, layer, ptr, i);
    break;
  }

  _spinlock_release(&zone->lock);
}

// Allocs physical page
void *__alloc_pages(int order, int flags) {
  return VTP(alloc_pages(order, flags));
}

// Frees physical page
void __free_pages(void *addr, int order, int flags) {
  free_pages(PTV(addr), order, flags);
}

// Allocs virtual identity mapped page
void *alloc_pages(int order, int flags) {
  struct zone *zone = get_zone(flags);
  int desired_order = order;
  order = -1;
  struct page *page = NULL;
  pageptr_t ptr = 0;

  spinlock_acquire(&zone->lock);

  // Get page from free list
  for (int i = desired_order; i < MAX_ORDER; i++) {
    struct buddy_layer *layer = &zone->buddy[i];
    if (layer->freelist.next != &layer->freelist) {
      page = layer->freelist.next;
      order = i;
      freelist_remove(page);

      // Get page out of bitmap
      ptr = __get_page_pointer(page, flags & ZONE_MASK);
      size_t index = ptr / (1 << (i + 1));
      flip_bit_in_ptr(layer->data, index);

      break;
    }
  }

  if (order == -1) {
    spinlock_release(&zone->lock);
    return NULL;
  }

  // Split downwards
  //
  // It took me way to fucking long to releaize it should be:
  // int i = order - 1; i >= desired_order; i--
  // and not:
  // int i = order; i > desired_order; i--
  for (int i = order - 1; i >= desired_order; i--) {
    // Get partner and free it
    pageptr_t partner_ptr = ptr ^ (1 << i);
    struct page *partner = __get_page_struct(partner_ptr, flags & ZONE_MASK);

    free_in_layer(partner, &zone->buddy[i], partner_ptr, i);
  }

  spinlock_release(&zone->lock);

  const size_t index =
      ((uintptr_t)page - PAGE_STRUCT_OFFSET) / sizeof(struct page);
  const uintptr_t paddr = index * PAGE_SIZE;

  return (void *)(paddr + IDENTITY_OFFSET);
}

// Frees virtual identity mapped page
void free_pages(void *addr, int order, int flags) {
  if (addr == NULL) {
    return;
  }

  struct zone *zone = NULL;
  int zone_index = 0;
  addr = VTP(addr);

  for (int i = 0; i < ZONE_COUNT; i++) {
    struct zone *c_zone = __get_zone(i);
    if ((uintptr_t)addr >= (uintptr_t)c_zone->base &&
        (uintptr_t)addr < (uintptr_t)c_zone->base + c_zone->length) {
      zone = c_zone;
      zone_index = i;
      break;
    }
  }

  if (!zone) {
    return;
  }

  struct page *page = __paddr_page_struct(addr);
  pageptr_t ptr = __get_page_pointer(page, zone_index);

  spinlock_acquire(&zone->lock);

  // At each level check if the partner is free, if so merge, if not keep going
  // Insert if you are at the top level (MAX_ORDER - 1)
  for (int i = order; i < MAX_ORDER; i++) {
    struct buddy_layer *layer = &zone->buddy[i];
    if (i == MAX_ORDER - 1) {
      free_in_layer(page, layer, ptr, i);
      break;
    }

    size_t index = ptr / (1 << (i + 1));
    if (check_bit_in_ptr(layer->data, index)) {
      // The partner is free
      pageptr_t partner_ptr = ptr ^ (1 << i);
      struct page *partner = __get_page_struct(partner_ptr, zone_index);

      // Allocate partner
      freelist_remove(partner);
      flip_bit_in_ptr(layer->data, index);

      bool older = partner_ptr < ptr;
      ptr = older ? partner_ptr : ptr;
      page = older ? partner : page;

      continue;
    }

    // The partner is allocated: put this into the layer
    free_in_layer(page, layer, ptr, i);
    break;
  }

  spinlock_release(&zone->lock);
}
