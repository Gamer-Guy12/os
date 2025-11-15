#include "lib/spinlock.h"
#include "interrupts.h"
#include "lib/atomic.h"
#include <stdbool.h>

void spinlock_acquire(spinlock_t *spinlock) {
  disable_interrupts();
  while (!atomic_cas(spinlock, 0, 1)) {
    __asm__ volatile("pause" ::: "memory");
  }
}

bool spinlock_attempt(spinlock_t *spinlock) {
  disable_interrupts();
  if (atomic_cas(spinlock, 0, 1)) {
    return true;
  }
  enable_interrupts();
  return false;
}

void spinlock_release(spinlock_t *spinlock) {
  atomic_store(spinlock, 0);
  enable_interrupts();
}
