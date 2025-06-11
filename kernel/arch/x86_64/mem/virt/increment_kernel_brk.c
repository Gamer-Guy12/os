#include <libk/math.h>
#include <libk/spinlock.h>
#include <mem/pimemory.h>
#include <mem/vimemory.h>
#include <stddef.h>

void *increment_kernel_brk(vmm_kernel_region_t *region, uint64_t amount) {
  spinlock_acquire(&region->brk_lock);

  // Check how many pages to map
  const size_t pages_to_map =
      ((ROUND_UP((size_t)region->brk + amount, PAGE_SIZE)) -
       (ROUND_UP((size_t)region->brk, PAGE_SIZE))) /
      PAGE_SIZE;

  if (pages_to_map == 0) {
    // im a nice guy
    // goto ret;
    region->brk = (void *)((size_t)region->brk + amount);
    void *ret = region->brk;
    spinlock_release(&region->brk_lock);
    return ret;
  }

  size_t cur_addr = ROUND_DOWN((size_t)region->brk, PAGE_SIZE);
  for (size_t i = 0; i < pages_to_map; i++) {
    cur_addr += PAGE_SIZE;
    map_page((void *)cur_addr, PT_PRESENT | PT_READ_WRITE, 1);
  }

  // ret:
  region->brk = (void *)((size_t)region->brk + amount);
  void *ret = region->brk;
  spinlock_release(&region->brk_lock);
  return ret;
}
