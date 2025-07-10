#include <libk/math.h>
#include <libk/mem.h>
#include <libk/spinlock.h>
#include <mem/gheap.h>
#include <mem/memory.h>
#include <stddef.h>
#include <x86_64.h>

gheap_entry_t *find_entry(size_t size) {
  gheap_entry_t *ptr = get_last_entry();

  if (ptr == NULL) return NULL;

  while (!ptr->free || (ptr->useless << 3) < size) {
    if (ptr->next == NULL) {
      return NULL;
    }

    ptr = ptr->next;
  }

  return NULL;
}

gheap_entry_t *create_entry(size_t size) {
  gheap_entry_t *last = get_last_entry();
  gheap_entry_t *entry = increment_global_brk(0);

  increment_global_brk(size + sizeof(gheap_entry_t));

  entry->size = size;
  entry->free = 0;
  entry->next = last;

  add_entry(entry);

  return entry;
}

void *gmalloc(size_t size) {
  spinlock_acquire(get_heap_lock());

  size = ROUND_UP(size, 8);
  gheap_entry_t *entry = NULL;
  find_entry(size);

  if (entry == NULL)
    entry = create_entry(size);

  spinlock_release(get_heap_lock());

  memset(entry + 1, 0, entry->useless << 3);

  return entry + 1;
}
