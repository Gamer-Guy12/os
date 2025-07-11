#include <libk/math.h>
#include <libk/mem.h>
#include <libk/spinlock.h>
#include <mem/kheap.h>
#include <mem/memory.h>
#include <stddef.h>

heap_entry_t *free_list = NULL;
heap_entry_t *used_list = NULL;

static heap_entry_t *find_entry(size_t size) {}

static heap_entry_t *create_entry(size_t size) {

}

void *kmalloc(size_t size, uint8_t flags) {
  vmm_kernel_region_t *region = *KERNEL_REGION_PTR_LOCATION;
  spinlock_acquire(&region->brk_lock);

  size = ROUND_UP(size, 8);

  heap_entry_t *entry = find_entry(size);

  if (entry == NULL)
    entry = create_entry(size);

  spinlock_release(&region->brk_lock);

  memset(entry + 1, 0, entry->size);

  return entry + 1;
}
