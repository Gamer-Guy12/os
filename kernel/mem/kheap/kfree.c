#include <libk/spinlock.h>
#include <mem/kheap.h>
#include <mem/memory.h>
#include <stddef.h>

void kfree(void *addr) {
  vmm_kernel_region_t *region = *KERNEL_REGION_PTR_LOCATION;
  spinlock_acquire(&region->global_lock);

  heap_entry_t *entry = (void *)((size_t)addr - sizeof(heap_entry_t));
  entry->free = 1;

  spinlock_release(&region->global_lock);
}
