#include "kernel/mem.h"
#include "kernel/kprintf.h"
#include "lib/bit.h"
#include "lib/spinlock.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static struct zone *get_zone(int flags) {
  int zone_portion = flags & ZONE_MASK;
  return __get_zone(zone_portion);
}

// Gets it's partner
static pageptr_t __split_page(struct page *page, int zone, int order) {
  pageptr_t page_ptr = __get_page_pointer(page, zone);
  pageptr_t partner_ptr = page_ptr ^ (1 << order);
  return partner_ptr;
}

static void free_into_layer(struct zone *zone, struct page *page, pageptr_t ptr, int order) {
  // Add page to freelist and set bit
  struct buddy_layer *free_layer = &zone->buddy[order];
  page->next = free_layer->freelist.next;
  page->prev = &free_layer->freelist;
  free_layer->freelist.next = page;
  page->next->prev = page;

  size_t index = ptr / (1 << (order + 1));
  flip_bit_in_ptr(free_layer->data, index);
}

static void freelist_remove(struct page *page) {
  page->next->prev = page->prev;
  page->prev->next = page->next;
}

// Can fail (returns null)
void *_alloc_page(int flags) {
  struct zone *zone = get_zone(flags & ZONE_MASK);
  const int desired_order = 0;
  int order = -1;
  struct page *page = NULL;

  _spinlock_acquire(&zone->lock);
  // Retrive page from freelist that is big enough but the smallest it can be
  for (int i = desired_order; i < MAX_ORDER; i++) {
    struct buddy_layer *layer = &zone->buddy[i];
    // Attempt free
    // Since the list is circular the list is empty if the freelist->next is the freelist itself
    if (layer->freelist.next != &layer->freelist) {
      page = layer->freelist.next;
      freelist_remove(layer->freelist.next);
      order = i;
  
      // Alloc page in bit mask
      pageptr_t ptr = __get_page_pointer(page, flags & ZONE_MASK);
      size_t index = ptr / (1 << (i + 1));
      flip_bit_in_ptr(layer->data, index);

      break;
    }
  }

  if (order == -1) {
    _spinlock_release(&zone->lock);
    return NULL;
  }

  // Iterate down splitting and setting bits
  // No failing allowed from here
  // If order == desired order no splitting required
  for (int i = order; i > desired_order; i--) {
    pageptr_t partner_ptr = __split_page(page, flags & ZONE_MASK, i);

    free_into_layer(zone, __get_page_struct(partner_ptr, flags & ZONE_MASK), partner_ptr, i - 1);
  }

  _spinlock_release(&zone->lock);

  uintptr_t page_addr = (uintptr_t)page - PAGE_STRUCT_OFFSET;
  page_addr /= sizeof(struct page);
  page_addr *= PAGE_SIZE;
  page_addr += IDENTITY_OFFSET;
  return (void *)page_addr;
}

// Can't fail
void _free_page(void *addr, int flags) {
  flags = flags & ~ZONE_MASK;
  // Find zone
  for (int i = ZONE_COUNT - 1; i >= 0; i--) {
    if ((uintptr_t)__get_zone(i)->base < (uintptr_t)VTP(addr)) {
      flags |= i;
      break;
    }
  }

  struct page *page = __paddr_page_struct(VTP(addr));
  pageptr_t page_ptr = __get_page_pointer(page, flags & ZONE_MASK);
  struct zone *zone = __get_zone(flags & ZONE_MASK);
  const size_t order = 0;

  page->next = NULL;
  page->prev = NULL;

  _spinlock_acquire(&zone->lock);
  // Free into bottom layer
  free_into_layer(zone, page, page_ptr, order);

  // Merge up the tree until impossible
  // Nodes in the top layer should not be merged so we are doing MAX_ORDER - 1
  for (int i = order; i < MAX_ORDER - 1; i++) {
    struct buddy_layer *layer = &zone->buddy[i];
    if (check_bit_in_ptr(layer->data, page_ptr / (1 << (i + 1)))) break;

    pageptr_t partner_ptr = page_ptr ^ (1 << i);
    struct page *partner = __get_page_struct(partner_ptr, flags & ZONE_MASK);

    freelist_remove(page);
    freelist_remove(partner);

    // called older because it is the "older" sibling
    bool older = page_ptr > partner_ptr;
    // Free into layer above
    free_into_layer(zone, older ? page : partner, older ? page_ptr : partner_ptr, i + 1);

    if (!older) {
      page_ptr = partner_ptr;
      page = partner;
    }
  }
  _spinlock_release(&zone->lock);
}
