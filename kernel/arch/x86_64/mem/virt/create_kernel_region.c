#include <libk/mem.h>
#include <libk/spinlock.h>
#include <mem/memory.h>
#include <mem/pimemory.h>
#include <mem/vimemory.h>
#include <stdatomic.h>

void create_kernel_region(vmm_kernel_region_t *region) {
  atomic_flag_clear_explicit(&region->stack_lock, memory_order_release);
  atomic_flag_clear_explicit(&region->brk_lock, memory_order_release);
  atomic_flag_clear_explicit(&region->mmap_lock, memory_order_release);
  atomic_flag_clear_explicit(&region->autogen_lock, memory_order_release);

  // IMPORTANT: Change this when u add mmap
  region->mmap_regions = NULL;

#define KERNEL_FREE_START (260ull * 512ull * GB + (0xffffull << 48))
#define KERNEL_FREE_END (509ull * 512ull * GB + (0xffffull << 48))
/// Not really middle but pretty much
#define KERNEL_FREE_MID (359ull * 512ull * GB + (0xffffull << 48))

  // Map the first page
  // It starts after a page because a pointer to the region will be stored at
  // the beginning
  region->start_brk = (void *)(KERNEL_FREE_START + PAGE_SIZE);
  region->brk = region->start_brk;
  map_page(region->start_brk, PT_READ_WRITE | PT_PRESENT, 1);
  memset(region->start_brk, 0, PAGE_SIZE);

  // Store said pointer
  map_page((void *)KERNEL_FREE_START, PT_READ_WRITE | PT_READ_WRITE, 1);
  vmm_kernel_region_t **start = (vmm_kernel_region_t **)(KERNEL_FREE_START);
  *start = region;

  region->start_autogen = (void *)(KERNEL_FREE_MID - 1);
  region->end_autogen = region->start_autogen;

  region->start_mmap = (void *)KERNEL_FREE_MID;
  region->end_mmap = region->start_mmap;

  region->start_stack = (void*)KERNEL_FREE_END;
  region->end_stack = (void*)(KERNEL_FREE_END - PAGE_SIZE);
  map_page(region->end_stack, PT_PRESENT | PT_READ_WRITE, 1);
  memset(region->end_stack, 0, PAGE_SIZE);
}
