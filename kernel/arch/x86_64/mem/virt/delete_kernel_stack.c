#include <mem/vimemory.h>
#include <mem/pimemory.h>
#include <mem/memory.h>

void delete_kernel_stack(size_t stack_index, vmm_kernel_region_t* region) {
  size_t stack_addr = (size_t)region->stacks_bottom - PAGE_SIZE * 2 * stack_index;
  void* addr1 = (void*)(stack_addr - PAGE_SIZE);
  void* addr2 = (void*)(stack_addr - PAGE_SIZE * 2);

  unmap_page(addr1, 1);
  unmap_page(addr2, 1);
}

