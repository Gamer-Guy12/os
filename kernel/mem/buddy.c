#include "kernel/kprintf.h"
#include "kernel/mem.h"
#include "lib/bit.h"
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

    for (int j = 0; j < zones[i].max_order; j++) {
      zones[i].freelists[j].buddy_data = alloc_buddy_data(byte_count);

      byte_count = (byte_count - 1) / 2 + 1;
    }
  }
}

INIT void init_buddy(void) {
  for (int i = 0; i < ZONE_COUNT; i++) {
    get_zone_info(get_zone(i), &zones[i].start, &zones[i].end,
                  &zones[i].max_order);

    for (int j = 0; j < zones[i].max_order; j++) {
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

static void __free_page_index(int zone, uint64_t page_index, uint32_t order) {
  size_t bit_index = page_to_zone_index(page_index, zone) >> (order + 1);
  flip_bit_in_ptr(zones[zone].freelists[order].buddy_data, bit_index);
  if (check_bit_in_ptr(zones[zone].freelists[order].buddy_data, bit_index)) {
    pages[page_index].next = zones[zone].freelists[order].freelist;
    if (pages[page_index].next != PAGE_NULL)
      pages[zones[zone].freelists[order].freelist].prev = page_index;
    zones[zone].freelists[order].freelist = page_index;
    return;
  }

  page_index &= ~(1 << order);

  for (int i = order + 1; i < zones[zone].max_order; i++) {
    uint64_t partner_zone_index =
        page_to_zone_index(page_index, zone) ^ (1 << i);
    uint64_t partner_index = zone_to_page_index(partner_zone_index, zone);
    uint64_t low_index =
        partner_zone_index < page_to_zone_index(page_index, zone)
            ? partner_index
            : page_index;
    page_index = low_index;
    uint64_t bit_index = (page_to_zone_index(low_index, zone) >> (i + 1));

    if (pages[partner_index].next != PAGE_NULL)
      pages[pages[partner_index].next].prev = pages[partner_index].prev;
    if (pages[partner_index].prev != PAGE_NULL)
      pages[pages[partner_index].prev].next = pages[partner_index].next;
    else
      zones[zone].freelists[i - 1].freelist = pages[partner_index].next;

    pages[partner_index].next = PAGE_NULL;
    pages[partner_index].prev = PAGE_NULL;

    pages[low_index].next = zones[zone].freelists[i].freelist;
    pages[low_index].prev = PAGE_NULL;
    zones[zone].freelists[i].freelist = low_index;

    flip_bit_in_ptr(zones[zone].freelists[i].buddy_data, bit_index);
    if (check_bit_in_ptr(zones[zone].freelists[i].buddy_data, bit_index)) {
      return;
    }
  }
}

void __free_page(void *addr, uint32_t order) {
  int zone = ZONE_COUNT - 1;
  for (int i = zone; i >= 0; i--) {
    if ((uintptr_t)addr < (uintptr_t)zones[i].end)
      zone = i;
  }

  uint64_t page_index = (uintptr_t)addr / PAGE_SIZE;

  spinlock_acquire(&zones[zone].lock);
  __free_page_index(zone, page_index, order);
  spinlock_release(&zones[zone].lock);
}

uint64_t alloc_from_freelist(int zone, uint32_t order) {
  struct buddy_data *freelist = &zones[zone].freelists[order];

  if (freelist->freelist == PAGE_NULL)
    return PAGE_NULL;

  uint64_t page_index = freelist->freelist;
  freelist->freelist = pages[page_index].next;
  const uint64_t zone_page_index = page_to_zone_index(page_index, zone);
  const uint64_t bit_index = zone_page_index >> (order + 1);
  flip_bit_in_ptr(freelist->buddy_data, bit_index);

  return page_index;
}

uint64_t __alloc_page_index(int zone, uint32_t order) {
  uint64_t page_index = PAGE_NULL;
  uint32_t page_order = 0;

  for (int i = order; i < zones[zone].max_order; i++) {
    page_index = alloc_from_freelist(zone, i);

    if (page_index != PAGE_NULL) {
      page_order = i;
      break;
    }
  }

  if (page_index == PAGE_NULL)
    return PAGE_NULL;

  for (int i = page_order; i > order; i--) {
    uint64_t partner_index = page_index ^ (1 << i);
    __free_page_index(zone, partner_index, i);
  }

  return page_index;
}

void *__alloc_page(uint32_t order, uint32_t zone) {
  uint64_t page_index = PAGE_NULL;

  spinlock_acquire(&zones[zone].lock);
  page_index = __alloc_page_index(zone, order);
  spinlock_release(&zones[zone].lock);

  if (page_index == PAGE_NULL)
    return NULL;

  return (void *)(uint64_t)(page_index * PAGE_SIZE);
}
