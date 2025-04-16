#include "./x86_64_pmm.h"
#include "libk/bit.h"
#include "libk/lock.h"
#include <libk/mem.h>
#include <pmm.h>
#include <stddef.h>
#include <stdint.h>

pageframe_t allocate_page_frame(void) {
  lock_acquire(get_mem_lock());
  size_t i = 0;
  for (i = 0; i < page_bitmap_end - page_bitmap_start; i++) {
    if (page_bitmap_start[i] != 0xFF) {
      break;
    }
  }

  uint8_t bit = find_zero8(page_bitmap_start[i]);
  page_bitmap_start[i] |= 1 << bit;
  uint8_t *addr = (uint8_t *)((i * 8 + bit) * 4096);

  lock_release(get_mem_lock());
  return addr;
}
