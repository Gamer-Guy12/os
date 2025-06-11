#include <libk/mem.h>
#include <libk/spinlock.h>
#include <mem/pimemory.h>
#include <mem/vimemory.h>

void create_kernel_region(vmm_kernel_region_t *region) {
  spinlock_acquire(&region->global_lock);

  // IMPORTANT: Change this when u add mmap
  region->mmap_regions = NULL;

#define KERNEL_FREE_START (259ull * 512ull * GB + (0xffffull << 48))
#define KERNEL_FREE_END (509ull * 512ull * GB + (0xffffull << 48))
/// Not really middle but pretty much
#define KERNEL_FREE_MID (359ull * 512ull * GB + (0xffffull << 48))

  // Map the first page
  region->start_brk = (void *)KERNEL_FREE_START;
  region->brk = region->start_brk;
  map_page(region->start_brk, PT_READ_WRITE | PT_PRESENT, 1);

  region->start_autogen = (void *)(KERNEL_FREE_MID - 1);
  region->start_autogen = region->end_autogen;

  region->start_mmap = (void *)KERNEL_FREE_MID;
  region->end_mmap = region->start_mmap;

  spinlock_release(&region->global_lock);
}
