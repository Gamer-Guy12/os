#include "./x86_64_pmm.h"
#include "libk/bit.h"
#include "libk/lock.h"
#include <arch/pmm.h>
#include <libk/mem.h>
#include <stddef.h>
#include <stdint.h>

pageframe_t allocate_page_frame(void) {
  lock_acquire(get_mem_lock());
  size_t i = 0;
  for (i = 0; i < kernel_gp_end - kernel_gp; i++) {
    if (kernel_gp[i] != 0xFF) {
      break;
    }
  }

  uint8_t bit = find_zero8(kernel_gp[i]);
  kernel_gp[i] |= 1 << bit;
  uint8_t *addr = (uint8_t *)((i * 8 + bit) * 4096);

  lock_release(get_mem_lock());
  return addr;
}
