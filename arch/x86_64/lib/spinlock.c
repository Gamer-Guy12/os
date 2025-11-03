#include "lib/spinlock.h"
#include "interrupts.h"
#include "lib/atomic.h"

void spinlock_acquire(spinlock_t *spinlock) {
  disable_interrupts();
  while (!atomic_cas(spinlock, 0, 1)) {
    __asm__ volatile("pause" ::: "memory");
  }
}

void spinlock_release(spinlock_t *spinlock) {
  atomic_store(spinlock, 0);
  enable_interrupts();
}
