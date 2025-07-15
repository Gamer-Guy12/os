#include <libk/math.h>
#include <libk/mem.h>
#include <libk/spinlock.h>
#include <mem/memory.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

gheap_entry_t *free_list = NULL;
gheap_entry_t *used_list = NULL;

static spinlock_t lock = ATOMIC_FLAG_INIT;

/// The maximum number of free poitners it can search before it gives up
#define GMALLOC_MAX_SEARCH 32
/// if the size of the entry / GMALLOC_MAX_MULTIPLE is greater than the requested size it is too big
///
/// eg. if the size requested is 16 and GMALLOC_MAX_MULTIPLE is 2 then anything bigger than 32 (16 * 2) is too big
#define GMALLOC_MAX_MULTIPLE 2

static bool not_valid(gheap_entry_t *ptr, size_t size) {
  if (size > ptr->size) {
    return true;
  }

  if (ptr->size / GMALLOC_MAX_MULTIPLE > size) {
    return true;
  }

  return false;
}

/// Note this funciton fully formats the entry for use
gheap_entry_t *find_entry(size_t size) {
  gheap_entry_t *ptr = free_list;

  if (ptr == NULL)
    return NULL;

  size_t search_count = 0;

  while (not_valid(ptr, size)) {
    if (search_count == GMALLOC_MAX_SEARCH) {
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

  ptr->free = 0;
  ptr->next = used_list;
  if (used_list != NULL)
    used_list->prev = ptr;
  used_list = ptr;

  return ptr;
}

gheap_entry_t *create_entry(size_t size) {
  gheap_entry_t *ptr = increment_global_brk(0);
  increment_global_brk(size + sizeof(gheap_entry_t));

  ptr->size = size;
  ptr->free = 0;
  ptr->next = used_list;
  if (used_list != NULL)
    used_list->prev = ptr;
  ptr->prev = NULL;

  return ptr;
}

void *gmalloc(size_t size) {
  spinlock_acquire(&lock);

  size = ROUND_UP(size, 8);

  gheap_entry_t *entry = find_entry(size);

  if (entry == NULL)
    entry = create_entry(size);

  spinlock_release(&lock);

  memset(entry + 1, 0, entry->useless << 3);

  return entry + 1;
}

void gfree(void *ptr) {
  spinlock_acquire(&lock);

  gheap_entry_t *entry = (gheap_entry_t *)ptr - 1;
  entry->next = free_list;
  if (free_list != NULL)
    free_list->prev = entry;
  free_list = entry;
  entry->free = 1;
  entry->prev = NULL;

  spinlock_release(&lock);
}

