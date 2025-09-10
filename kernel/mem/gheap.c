#include <libk/math.h>
#include <libk/mem.h>
#include <libk/spinlock.h>
#include <mem/memory.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

gheap_entry_t *free_list = NULL;

static spinlock_t lock = ATOMIC_FLAG_INIT;

/// The maximum number of free poitners it can search before it gives up
#define GMALLOC_MAX_SEARCH 64
/// if the size of the entry / GMALLOC_MAX_MULTIPLE is greater than the
/// requested size it is too big
///
/// eg. if the size requested is 16 and GMALLOC_MAX_MULTIPLE is 2 then anything
/// bigger than 32 (16 * 2) is too big
#define GMALLOC_MAX_MULTIPLE 2
/// When gmalloc can't find an entry of the correct size it will create it.
///
/// This defines how many extras it should create.
///
/// For example if gmalloc can't find a block of size 32 bytes, it will create
/// one plus this many extras for use.
///
/// If the value is 3 then it will create 4 blocks of size 32: 1 for use and 3
/// more as extras
#define GMALLOC_EXTRAS_COUNT 3

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
  if (!ptr->prev)
    free_list = ptr->next;

  return ptr;
}

gheap_entry_t *create_entry(size_t size) {

  gheap_entry_t *ptr = (gheap_entry_t *)((size_t)increment_global_brk(
                                             size + sizeof(gheap_entry_t)) -
                                         (size + sizeof(gheap_entry_t)));

  ptr->size = size;

  /// Create the extras
  size_t extra_size = GMALLOC_EXTRAS_COUNT * (size + sizeof(gheap_entry_t));
  gheap_entry_t *extra_ptr =
      (gheap_entry_t *)((size_t)increment_global_brk(extra_size) - extra_size);

  for (size_t i = 0; i < GMALLOC_EXTRAS_COUNT; i++) {
    extra_ptr->size = size;

    extra_ptr->next = free_list;
    if (free_list != NULL)
      free_list->prev = extra_ptr;
    free_list = extra_ptr;
    extra_ptr->prev = NULL;

    extra_ptr = (void *)((size_t)extra_ptr + sizeof(gheap_entry_t) + size);
  }

  return ptr;
}

void *gmalloc(size_t size) {
  spinlock_acquire(&lock);

  size = ROUND_UP(size, 16);

  gheap_entry_t *entry = find_entry(size);

  if (entry == NULL)
    entry = create_entry(size);

  spinlock_release(&lock);

  memset(entry + 1, 0, size);

  return entry + 1;
}

void gfree(void *ptr) {
  spinlock_acquire(&lock);

  gheap_entry_t *entry = (gheap_entry_t *)ptr - 1;

  entry->next = free_list;
  if (free_list != NULL)
    free_list->prev = entry;
  free_list = entry;
  entry->prev = NULL;

  spinlock_release(&lock);
}
