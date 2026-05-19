#include "kernel/fmem.h"
#include "kernel/kprintf.h"
#include "kernel/mem.h"
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

  if (entry) {
    normal_pages = entry->next;
    return (void *)entry;
  } else {
    if (large_pages) {
      entry = large_pages;
      large_pages = entry->next;

      return (void *)__split_large(entry);
    } else {
      return NULL;
    }
  }
}

void _fmem_free(void *addr) {
  struct fmem_entry *entry = addr;
  entry->next = normal_pages;
  normal_pages = entry;
}

void _fmem_add_range(void *start, void *end) {
  size_t size = (uintptr_t)end - (uintptr_t)start;
  uintptr_t cur_ptr = (uintptr_t) start;
  size_t current_size = (PAGE_SIZE * (1 << (MAX_ORDER - 1)));

  for (int i = MAX_ORDER; i >= 0; i--) {
    while (size > current_size) {
      size -= current_size;
      struct fmem_entry *entry = (struct fmem_entry*)cur_ptr;
      entry->page_order = i;
      entry->next = i == 0 ? normal_pages : large_pages;
      if (i == 0) {
        normal_pages = entry;
      } else {
        large_pages = entry;
      }

      cur_ptr += current_size;
    }
  }
}
