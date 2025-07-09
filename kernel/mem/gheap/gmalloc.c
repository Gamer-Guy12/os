#include <mem/memory.h>
#include <libk/math.h>
#include <libk/spinlock.h>
#include <mem/gheap.h>
#include <stddef.h>
#include <libk/mem.h>

gheap_entry_t *find_entry(size_t size) {
  gheap_entry_t *start = get_last_entry();

  gheap_entry_t *ptr = start;

  if (start == NULL) return start;

  do {
    if (ptr->size >= size && ptr->free) {
      ptr->free = 0;
      return ptr;
    }

    ptr = ptr->next;
  } while (ptr->next != NULL);

  return NULL;
}

gheap_entry_t* create_entry(size_t size) {
  gheap_entry_t* last = get_last_entry();
  gheap_entry_t* entry = increment_global_brk(0);

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
  gheap_entry_t* entry = find_entry(size);

  if (entry == NULL) entry = create_entry(size);

  spinlock_release(get_heap_lock());

  memset(entry + 1, 0, entry->size);

  return entry + 1;
}
