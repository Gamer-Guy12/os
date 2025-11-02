#include "lib/spinlock.h"
#include "lib/atomic.h"

void spinlock_acquire(spinlock_t *spinlock) {
  while (!atomic_cas(spinlock, 0, 1)) {
#ifdef _x86_64_
    __asm__ volatile("pause" ::: "memory");
#endif
  }
}

void spinlock_release(spinlock_t *spinlock) { atomic_store(spinlock, 0); }

