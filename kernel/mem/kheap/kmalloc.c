#include <libk/math.h>
#include <libk/spinlock.h>
#include <mem/kheap.h>
#include <mem/memory.h>
#include <stddef.h>

/// First fit not best fit
/// Last chunk is set if it can't find a free chunk
heap_entry_t *find_free_chunk(size_t size, heap_entry_t **last_chunk) {
  vmm_kernel_region_t *region = *KERNEL_REGION_PTR_LOCATION;

  heap_entry_t *entry = region->start_brk;

  while (!entry->free || entry->size < size) {
    if (entry->next == NULL) {
      *last_chunk = entry;
      return NULL;
    }

    entry = entry->next;
  }

  return entry;
}

heap_entry_t *create_chunk(size_t size, heap_entry_t *last_chunk) {
  vmm_kernel_region_t *region = *KERNEL_REGION_PTR_LOCATION;

  heap_entry_t *new = increment_kernel_brk(region, 0);
  increment_kernel_brk(region, size + sizeof(heap_entry_t));

  new->size = size;
  new->next = NULL;
  new->free = 0;

  last_chunk->next = new;

  return new;
}

void *kmalloc(size_t size, uint8_t flags) {
  vmm_kernel_region_t *region = *KERNEL_REGION_PTR_LOCATION;
  size_t actual_size = ROUND_UP(size, 8);

  spinlock_acquire(&region->brk_lock);

  heap_entry_t *last_chunk = NULL;
  heap_entry_t *chunk = find_free_chunk(actual_size, &last_chunk);

  if (!chunk) {
    chunk = create_chunk(actual_size, last_chunk);
  }

  spinlock_release(&region->brk_lock);

  // + 1 to skip over the metadata
  return (void *)(chunk + 1);
}
