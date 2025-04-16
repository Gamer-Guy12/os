#include "./x86_64_pmm.h"
#include "libk/lock.h"
#include <libk/bit.h>
#include <libk/mem.h>
#include <pmm.h>
#include <stddef.h>
#include <stdint.h>

void free_page_frame(pageframe_t *page) {
  lock_acquire(get_mem_lock());
  size_t index = (size_t)page / 4096;
  uint8_t bit = index % 8;
  index = (index - bit) / 8;

  page_bitmap_start[index] &= ~(1 << bit);
  lock_release(get_mem_lock());
}
