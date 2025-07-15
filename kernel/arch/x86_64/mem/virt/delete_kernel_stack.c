#include <mem/vimemory.h>
#include <mem/memory.h>
#include <mem/pimemory.h>

void delete_kernel_stack(size_t stack_num, vmm_kernel_region_t *region) {

  size_t stack_addr = (size_t)region->stacks_bottom;
  stack_addr -= 2 * PAGE_SIZE * stack_num;
  void *addr1 = (void *)(stack_addr - PAGE_SIZE);
  void *addr2 = (void *)(stack_addr - PAGE_SIZE * 2);

  unmap_page(addr1, true);
  unmap_page(addr2, true);
}

