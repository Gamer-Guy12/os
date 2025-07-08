#include <libk/spinlock.h>
#include <stdatomic.h>

void spinlock_release(spinlock_t *lock) {
  atomic_flag_clear_explicit(lock, memory_order_release);
}

