#include <hal/pimemory.h>
#include <hal/vimemory.h>
#include <libk/math.h>
#include <stddef.h>

void *increment_kernel_brk(vmm_kernel_region_t *region, uint64_t amount) {
  // Check how many pages to map
  const size_t pages_to_map =
      ((ROUND_UP((size_t)region->brk + amount, PAGE_SIZE)) -
       (ROUND_UP((size_t)region->brk, PAGE_SIZE))) /
      PAGE_SIZE;

  if (pages_to_map == 0) {
    // im a nice guy
    // goto ret;
    region->brk = (void *)((size_t)region->brk + amount);
    return region->brk;
  }

  size_t cur_addr = ROUND_DOWN((size_t)region->brk, PAGE_SIZE);
  for (size_t i = 0; i < pages_to_map; i++) {
    while (1) {
    }
    map_page((void *)cur_addr, PT_PRESENT | PT_READ_WRITE, 1);
    cur_addr += PAGE_SIZE;
  }

  // ret:
  region->brk = (void *)((size_t)region->brk + amount);
  return region->brk;
}
