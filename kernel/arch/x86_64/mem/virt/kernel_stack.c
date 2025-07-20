#include <libk/spinlock.h>
#include <mem/memory.h>
#include <mem/pimemory.h>
#include <mem/vimemory.h>
#include <stddef.h>

void *create_new_kernel_stack(vmm_kernel_region_t *region, bool map, size_t* stack_index) {
  *stack_index = 0;
  size_t stack_addr = (size_t)region->stacks_bottom;

  spinlock_acquire(&region->stack_lock);
  *stack_index = region->stack_index;
  region->stack_index++;
  spinlock_release(&region->stack_lock);

  stack_addr -= 2 * PAGE_SIZE * *stack_index;

  void *addr1 = (void *)(stack_addr - PAGE_SIZE);
  void *addr2 = (void *)(stack_addr - PAGE_SIZE * 2);

  if (map) {
    map_page(addr1, PT_PRESENT | PT_READ_WRITE, 1);
    map_page(addr2, PT_PRESENT | PT_READ_WRITE, 1);
  }

  return (void *)(stack_addr - 8);
}

void *delete_kernel_stack(size_t stack_index, vmm_kernel_region_t *region,
                          bool unmap) {
  if (!unmap) {
    size_t stack_addr =
        (size_t)region->stacks_bottom - 2 * PAGE_SIZE * stack_index;

    void *addr2 = (void *)(stack_addr - PAGE_SIZE * 2);

    return addr2;
  }

  size_t stack_addr =
      (size_t)region->stacks_bottom - 2 * PAGE_SIZE * stack_index;

  void *addr1 = (void *)(stack_addr - PAGE_SIZE);
  void *addr2 = (void *)(stack_addr - PAGE_SIZE * 2);

  unmap_page(addr1, true);
  unmap_page(addr2, true);

  return addr2;
}

