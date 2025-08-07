#include <libk/spinlock.h>
#include <mem/memory.h>
#include <mem/pimemory.h>
#include <mem/vimemory.h>
#include <stddef.h>

#define STACKS_BOTTOM INDICES_TO_ADDR(0, 0, 0, 509ull)

size_t global_stack_index = 0;

void *create_new_kernel_stack(size_t *stack_index) {
  *stack_index = 0;
  size_t stack_addr = STACKS_BOTTOM;

  *stack_index =
      __atomic_fetch_add(&global_stack_index, 1, __ATOMIC_RELEASE);

  stack_addr -= 2 * PAGE_SIZE * *stack_index;

  void *addr1 = (void *)(stack_addr - PAGE_SIZE);
  void *addr2 = (void *)(stack_addr - PAGE_SIZE * 2);

  map_page(addr1, PT_PRESENT | PT_READ_WRITE, 1);
  map_page(addr2, PT_PRESENT | PT_READ_WRITE, 1);

  return (void *)(stack_addr - 8);
}

void *delete_kernel_stack(size_t stack_index) {
  size_t stack_addr =
      STACKS_BOTTOM - 2 * PAGE_SIZE * stack_index;

  void *addr1 = (void *)(stack_addr - PAGE_SIZE);
  void *addr2 = (void *)(stack_addr - PAGE_SIZE * 2);

  unmap_page(addr1, true);
  unmap_page(addr2, true);

  return addr2;
}
