#include "kernel/cls.h"
#include "kernel/cores.h"
#include "kernel/mem.h"
#include <stddef.h>
#include <stdint.h>

extern char _start_cls[];
extern char _end_cls[];

void *cls_tables = NULL;
size_t cls_stride = 0;

static inline size_t calculate_order(size_t size) {
  size_t page_count = ((size - 1) / PAGE_SIZE) + 1;
  size_t cur_order = 0;

  while ((1 << cur_order) < page_count) {
    cur_order++;
  }

  return cur_order;
}

void init_cls(void) {
  const size_t entry_count =
      ((uintptr_t)_end_cls - (uintptr_t)_start_cls) / sizeof(size_t);

  size_t *entries = (size_t *)_start_cls;
  size_t cur_size = 0;

  for (size_t i = 0; i < entry_count; i++) {
    entries[i] = cur_size;
    cur_size += entries[i];
  }

  if (cur_size == 0) {
    return;
  }

  cls_tables = alloc_pages(calculate_order(cur_size * get_core_count()), ZONE_ANY);
  cls_stride = cur_size;
}

