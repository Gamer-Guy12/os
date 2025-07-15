#include <libk/mem.h>
#include <libk/spinlock.h>
#include <mem/kheap.h>
#include <mem/memory.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_SEARCH_COUNT_KMALLOC 32
#define MAX_MULTIPLE_KMALLOC 2

bool not_valid(size_t size, heap_entry_t *entry) {
  if (size > entry->size) {
    return true;
  }

  if (entry->size / MAX_MULTIPLE_KMALLOC > size) {
    return true;
  }

  return false;
}

// Wholesale copied from gmalloc
heap_entry_t *find_heap_entry(size_t size, heap_info_t *info) {
  heap_entry_t *ptr = info->free_list;

  if (ptr == NULL)
    return NULL;

  size_t search_count = 0;

  while (not_valid(size, ptr)) {
    if (search_count == MAX_SEARCH_COUNT_KMALLOC) {
      return NULL;
    }

    if (ptr->next == NULL)
      return NULL;

    ptr = ptr->next;
    search_count++;
  }

  if (ptr->next)
    ptr->next->prev = ptr->prev;
  if (ptr->prev)
    ptr->prev->next = ptr->next;

  ptr->next = info->used_list;
  if (info->used_list != NULL) {
    info->used_list->prev = ptr;
  }
  info->used_list = ptr;

  return ptr;
}

heap_entry_t *create_heap_entry(size_t size, heap_info_t *info) {
  vmm_kernel_region_t *region = get_kernel_region();

  heap_entry_t *entry = increment_kernel_brk(region, 0);
  increment_kernel_brk(region, size + sizeof(heap_entry_t));

  entry->size = size;
  entry->next = info->used_list;
  entry->prev = NULL;

  if (info->used_list != NULL) {
    info->used_list->prev = entry;
  }

  info->used_list = entry;

  return entry;
}

void *kmalloc(size_t size, uint8_t flags) {
  heap_info_t *heap_info = get_heap_info();

  spinlock_acquire(&heap_info->heap_lock);

  heap_entry_t *entry = find_heap_entry(size, heap_info);

  if (entry == NULL) {
    entry = create_heap_entry(size, heap_info);
  }

  spinlock_release(&heap_info->heap_lock);

  memset(entry + 1, 0, size);

  return entry + 1;
}

// Also cloned from Gmalloc (this entire file is basically cloned)
void kfree(void* ptr) {
  heap_info_t* info = get_heap_info();

  spinlock_acquire(&info->heap_lock);

  heap_entry_t* entry = (heap_entry_t*)ptr - 1;
  entry->next = info->free_list;
  if (info->free_list != NULL)
    info->free_list->prev = entry; 
  info->free_list = entry;
  entry->prev = NULL; 

  spinlock_release(&info->heap_lock);
}

