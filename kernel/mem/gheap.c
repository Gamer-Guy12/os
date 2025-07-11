#include <libk/mem.h>
#include <libk/spinlock.h>
#include <mem/memory.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>

gheap_entry_t *free_list = NULL;
gheap_entry_t *used_list = NULL;

static spinlock_t lock = ATOMIC_FLAG_INIT;

/// Note this funciton fully formats the entry for use
gheap_entry_t *find_entry(size_t size) {

}

gheap_entry_t *create_entry(size_t size) {
  gheap_entry_t *ptr = increment_global_brk(0);
  increment_global_brk(size + sizeof(gheap_entry_t));

  ptr->size = size;
  ptr->free = 0;
  ptr->next = used_list;
  ptr->prev = NULL;

  return ptr;
}

void *gmalloc(size_t size) {
  spinlock_acquire(&lock);

  gheap_entry_t *entry = find_entry(size);

  if (entry == NULL)
    entry = create_entry(size);

  spinlock_release(&lock);

  memset(entry + 1, 0, entry->useless << 3);

  return entry + 1;
}
