#include <hal/vimemory.h>

void *move_kernel_brk(vmm_kernel_region_t *region, int64_t amount) {
  if (amount >= 0) {
    return increment_kernel_brk(region, amount);
  } else {
    return decrement_kernel_brk(region, amount);
  }
}
