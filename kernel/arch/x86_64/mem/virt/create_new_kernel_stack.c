#include <libk/spinlock.h>
#include <mem/memory.h>
#include <mem/pimemory.h>
#include <mem/vimemory.h>
#include <stddef.h>

void *create_new_kernel_stack(vmm_kernel_region_t *region, bool map) {
  size_t stack_index = 0;
  size_t stack_addr = (size_t)region->stacks_bottom;

  spinlock_acquire(&region->stack_lock);
  stack_index = region->stack_index;
  region->stack_index++;
  spinlock_release(&region->stack_lock);

  stack_addr -= 2 * PAGE_SIZE * stack_index;

  void *addr1 = (void *)(stack_addr - PAGE_SIZE);
  void *addr2 = (void *)(stack_addr - PAGE_SIZE * 2);

  if (map) {
    map_page(addr1, PT_PRESENT | PT_READ_WRITE, 1);
    map_page(addr2, PT_PRESENT | PT_READ_WRITE, 1);
  }

  return (void *)(stack_addr - 8);
}
