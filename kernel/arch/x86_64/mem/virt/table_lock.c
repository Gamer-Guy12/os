#include <libk/spinlock.h>
#include <mem/vimemory.h>
#include <stdatomic.h>

static spinlock_t lock = ATOMIC_FLAG_INIT;

spinlock_t* get_table_lock(void) {
  return &lock;
}

