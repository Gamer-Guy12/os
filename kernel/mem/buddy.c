#include "kernel/kprintf.h"
#include "kernel/mem.h"
#include "lib/bit.h"
#include "lib/spinlock.h"
#include "lib/string.h"
#include "util.h"
#include <stddef.h>
#include <stdint.h>

struct zone zones[ZONE_COUNT];
static INIT_DATA uintptr_t buddy_data_ptr = BUDDY_DATA_ADDR;

#define MIN(a, b) ((a) < (b) ? (a) : (b))

static INIT void *alloc_buddy_data(size_t size) {
  void *ret = (void *)buddy_data_ptr;
  buddy_data_ptr += size;
  memset(ret, 0, size);
  return ret;
}

static INIT void init_buddy_data(void) {
  for (int i = 0; i < ZONE_COUNT; i++) {
    if (zones[i].start == zones[i].end)
      continue;

    const size_t start = (size_t)zones[i].start;
    const size_t end = MIN((size_t)zones[i].end, get_max_addr() - 1);
    if (start >= end)
      continue;

    const size_t page_count = (end - start) / PAGE_SIZE;
    const size_t bit_count = (page_count - 1) / 2 + 1;
    size_t byte_count = (bit_count - 1) / 8 + 1;

    for (int j = 0; j < MAX_ORDER; j++) {
      zones[i].freelists[j].buddy_data = alloc_buddy_data(byte_count);

      byte_count = (byte_count - 1) / 2 + 1;
    }
  }
}

INIT void init_buddy(void) {
  for (int i = 0; i < ZONE_COUNT; i++) {
    get_zone_info(get_zone(i), &zones[i].start, &zones[i].end);

    for (int j = 0; j < MAX_ORDER; j++) {
      zones[i].freelists[j].buddy_data = NULL;
      zones[i].freelists[j].freelist = PAGE_NULL;
    }
  }

  init_buddy_data();
  kprintf("\t[MEM] Initialized Buddy Data\n");
}

static inline uint64_t page_to_zone_index(uint64_t page_index, int zone) {
  size_t addr = page_index * PAGE_SIZE;
  addr -= (size_t)zones[zone].start;
  return addr / PAGE_SIZE;
}

static inline uint64_t zone_to_page_index(uint64_t zone_index, int zone) {
  size_t addr = zone_index * PAGE_SIZE;
  addr += (size_t)zones[zone].start;
  return addr / PAGE_SIZE;
}

static void free_page_index(uint64_t page_index, int zone, uint32_t order) {
  uint64_t zone_page_index = page_to_zone_index(page_index, zone);

  // Flip the bottom bit
  const uint64_t bit_index = zone_page_index >> (order + 1);
  struct buddy_data *freelist = &zones[zone].freelists[order];
  flip_bit_in_ptr(freelist->buddy_data, bit_index);

  // Check if it has a partner and if it's partner is in use add it to the
  // freelist
  if (check_bit_in_ptr(freelist->buddy_data, bit_index)) {
    struct page *page = get_page(page_index);
    page->next = freelist->freelist;
    page->prev = PAGE_NULL;
    if (freelist->freelist != PAGE_NULL)
      get_page(freelist->freelist)->prev = page_index;
    freelist->freelist = page_index;

    return;
  }

  // In a loop starting at order + 1 and less than max order
  // At this point we are at order + 1 and one child is in a freelist
  //
  // Witin this loop page index and zone page index are updated each iteration
  for (uint32_t i = order + 1; i < MAX_ORDER; i++) {
    const uint64_t zone_partner_index = zone_page_index ^ (1 << (i - 1));
    const uint64_t partner_index = zone_to_page_index(zone_partner_index, zone);
    const uint64_t zone_parent_index = zone_page_index & ~((1 << i) - 1);
    const uint64_t parent_index = zone_to_page_index(zone_parent_index, zone);
    struct buddy_data *lower_freelist = &zones[zone].freelists[i - 1];
    struct buddy_data *upper_freelist = &zones[zone].freelists[i];
    struct page *partner = get_page(partner_index);
    struct page *parent = get_page(parent_index);

    // Remove the partner from the freelist
    if (partner->next != PAGE_NULL)
      get_page(partner->next)->prev = partner->prev;
    if (partner->prev != PAGE_NULL)
      get_page(partner->prev)->next = partner->next;
    else
      lower_freelist->freelist = partner->next;

    partner->next = PAGE_NULL;
    partner->prev = PAGE_NULL;

    // array If it is 1 then add it to the array Once the list is done, if you
    // haven't returned by then add it to the array
    const uint64_t bit_index = zone_parent_index >> (i + 1);
    flip_bit_in_ptr(upper_freelist->buddy_data, bit_index);
    if (check_bit_in_ptr(upper_freelist->buddy_data, bit_index)) {
      parent->next = upper_freelist->freelist;
      parent->prev = PAGE_NULL;
      if (upper_freelist->freelist != PAGE_NULL)
        get_page(upper_freelist->freelist)->prev = parent_index;
      upper_freelist->freelist = parent_index;

      return;
    }

    zone_page_index = zone_parent_index;
    page_index = zone_to_page_index(zone_page_index, zone);
  }

  freelist = &zones[zone].freelists[MAX_ORDER - 1];
  struct page *page = get_page(page_index);
  page->next = freelist->freelist;
  page->prev = PAGE_NULL;
  if (freelist->freelist != PAGE_NULL)
    get_page(freelist->freelist)->prev = page_index;
  freelist->freelist = page_index;
}

void __free_pages(void *addr, uint32_t order) {
  int zone = ZONE_COUNT;
  uintptr_t addr_bits = (uintptr_t)addr;
  uint64_t page_index = addr_bits / PAGE_SIZE;

  for (int i = 0; i < ZONE_COUNT; i++) {
    if (addr_bits < zones[i].end) {
      zone--;
    }
  }

  spinlock_acquire(&zones[zone].lock);
  free_page_index(page_index, zone, order);
  spinlock_release(&zones[zone].lock);
}

uint64_t alloc_from_freelist(uint32_t order, uint32_t zone) {
  struct buddy_data *freelist = &zones[zone].freelists[order];

  const page_ptr_t page_index = freelist->freelist;

  if (page_index == PAGE_NULL)
    return PAGE_NULL;

  freelist->freelist = get_page(page_index)->next;
  get_page(page_index)->next = PAGE_NULL;
  get_page(page_index)->prev = PAGE_NULL;

  return page_index;
}

static uint64_t alloc_page_index(uint32_t order, uint32_t zone) {
  page_ptr_t page_index = PAGE_NULL;
  uint32_t page_order = MAX_32;

  // First try to allocate from any frelist starting at order and going up
  for (uint32_t i = order; i < MAX_ORDER; i++) {
    page_index = alloc_from_freelist(i, zone);

    if (page_index != PAGE_NULL) {
      page_order = i;
      break;
    }
  }

  if (page_index == PAGE_NULL)
    return PAGE_NULL;

  uint64_t zone_page_index = page_to_zone_index(page_index, zone);

  // Then free one half of it while moving downwards till you get to order + 1
  for (uint32_t i = page_order; i > order; i--) {
    // Set partner bit
    uint64_t partner_zone_index = zone_page_index | (1 << (i - 1));
    free_page_index(zone_to_page_index(partner_zone_index, zone), zone, i - 1);
  }

  return page_index;
}

void *__alloc_pages(uint32_t order, uint32_t zone) {
  spinlock_acquire(&zones[zone].lock);
  uint64_t page_index = alloc_page_index(order, zone);
  spinlock_release(&zones[zone].lock);

  if (page_index == PAGE_NULL)
    return NULL;

  return (void *)(page_index * PAGE_SIZE);
}
