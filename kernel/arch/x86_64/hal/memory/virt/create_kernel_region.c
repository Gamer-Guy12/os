#include <hal/vimemory.h>
#include <libk/lock.h>
#include <libk/mem.h>

void create_kernel_region(vmm_kernel_region_t *region) {
  lock_acquire(&region->global_lock);

  // IMPORTANT: Change this when u add mmap
  region->mmap_regions = NULL;

#define KERNEL_FREE_START (259ull * 512ull * GB + (0xffffull << 48))
#define KERNEL_FREE_END (509ull * 512ull * GB + (0xffffull << 48))
/// Not really middle but pretty much
#define KERNEL_FREE_MID (359ull * 512ull * GB + (0xffffull << 48))

  region->start_brk = (void *)KERNEL_FREE_START;
  region->brk = region->start_brk;

  region->start_autogen = (void *)(KERNEL_FREE_MID - 1);
  region->start_autogen = region->end_autogen;

  region->start_mmap = (void *)KERNEL_FREE_MID;
  region->end_mmap = region->start_mmap;

  lock_release(&region->global_lock);
}
