#include <libk/spinlock.h>
#include <mem/gheap.h>
#include <stdatomic.h>

spinlock_t heap_lock = ATOMIC_FLAG_INIT;

spinlock_t* get_heap_lock(void) {
  return &heap_lock;
}

