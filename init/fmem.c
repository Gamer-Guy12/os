#include "kernel/fmem.h"
#include "kernel/kprintf.h"
#include "kernel/mem.h"
#include "lib/string.h"
#include "util.h"
#include <stddef.h>
#include <stdint.h>

struct fmem_entry {
  struct fmem_entry *next;
  size_t page_order;
};

// 1KB pages
static struct fmem_entry *normal_pages = NULL;
// Large multi page chunks
static struct fmem_entry *large_pages = NULL;

// The order is allowed to be bigger because it can be broken up later
// The way it works is we make big chunks and then just quickly split them up through some basic math
// This is better than repetitivly checking the size of entries even when not needed
#define UNBOUNDED_ORDER 32

static struct fmem_entry *__do_split(struct fmem_entry *entry) {
  if (entry->page_order == 0) {
    return entry;
  }

  const size_t offset = PAGE_SIZE * (1 << (entry->page_order - 1));
  struct fmem_entry *second_entry =
      (struct fmem_entry *)((uintptr_t)entry + offset);
  second_entry->page_order = entry->page_order - 1;

  if (second_entry->page_order == 0) {
    second_entry->next = normal_pages;
    normal_pages = second_entry;
  } else {
    second_entry->next = large_pages;
    large_pages = second_entry;
  }

  entry->page_order--;
  return entry;
}

static struct fmem_entry *__split_large(struct fmem_entry *entry) {
  if (!entry) {
    return NULL;
  }

  while (entry->page_order != 0) {
    entry = __do_split(entry);
  }

  return entry;
}

void *_fmem_alloc(void) {
  struct fmem_entry *entry = normal_pages;
  void *ret = NULL;

  if (entry) {
    normal_pages = entry->next;
    ret = (void *)entry;
  } else {
    if (large_pages) {
      entry = large_pages;
      large_pages = entry->next;

      ret = (void *)__split_large(entry);
    } else {
      return NULL;
      // Cannot return NULL
      // Sike
      _kprintf("FMEM Failed to alloc\n");
      panic();
    }
  }

  memset(ret, 0, PAGE_SIZE);
  return ret;
}

void *_fmem_phys(void) {
  // _fmem_alloc can't return null
  return VTP(_fmem_alloc());
}

void _fmem_free(void *addr) {
  struct fmem_entry *entry = addr;
  entry->next = normal_pages;
  normal_pages = entry;
}

static int determine_order(size_t page_count, size_t ptr) {
  // We aren't checking order 0 because that one is just the default if nothing
  // else works
  for (int i = UNBOUNDED_ORDER - 1; i > 0; i--) {
    size_t addr_mask = PAGE_SIZE * (1 << i) - 1;
    size_t count = 1 << i;
    // If this one won't work, move on (it won't work because the ptr isn't
    // aligned)
    if (ptr & addr_mask)
      continue;
    // If there aren't enough pages keep going
    if (page_count < count)
      continue;

    return i;
  }

  return 0;
}

void _fmem_add_range(void *start, void *end) {
  size_t size = (uintptr_t)end - (uintptr_t)start;
  uintptr_t cur_ptr = (uintptr_t)start;
  size_t cur_page_count = size / PAGE_SIZE;

  while (cur_page_count > 0) {
    int order = determine_order(cur_page_count, cur_ptr);
    struct fmem_entry *entry = (struct fmem_entry *)cur_ptr;
    entry->page_order = order;

    if (order == 0) {
      entry->next = normal_pages;
      normal_pages = entry;
    } else {
      entry->next = large_pages;
      large_pages = entry;
    }

    cur_page_count -= 1 << order;
    cur_ptr += (1 << order) * PAGE_SIZE;
  }
}

static void free_large_entry(struct fmem_entry *entry) {
  if (entry->page_order < MAX_ORDER) {
    _free_pages(entry, entry->page_order, 0);
    return;
  }

  // Split page
  // If the order is 10 that means there are 2 portions so entry->page_order -
  // MAX_ORDER + 1 = 1 and then 1 << 1 = 2
  size_t portions = 1 << (entry->page_order - MAX_ORDER + 1);
  size_t step = PAGE_SIZE * (1 << (MAX_ORDER - 1));
  uintptr_t addr = (uintptr_t)entry;
  const int order = MAX_ORDER - 1;

  for (size_t i = 0; i < portions; i++) {
    _free_pages((void *)addr, order, 0);
    addr += step;
  }
}

void free_fmem(void) {
  while (normal_pages) {
    struct fmem_entry *entry = normal_pages;
    normal_pages = entry->next;
    _free_pages(entry, 0, 0);
  }

  while (large_pages) {
    struct fmem_entry *entry = large_pages;
    large_pages = entry->next;
    free_large_entry(entry);
  }
}
