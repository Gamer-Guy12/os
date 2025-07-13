#include <libk/math.h>
#include <libk/mem.h>
#include <mem/kheap.h>
#include <mem/memory.h>

void init_heap(void) {
  vmm_kernel_region_t *region = get_kernel_region();

  // Align the heap to 8 bytes
  if ((size_t)region->start_brk != 0) {
    size_t increment =
        ROUND_UP((size_t)region->start_brk, 8) - (size_t)region->start_brk;

    increment_kernel_brk(region, increment);
  }
}
