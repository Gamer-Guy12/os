#include "lib/rw_lock.h"
#include "interrupts.h"
#include "lib/atomic.h"
#include <stdbool.h>

void rw_read_acquire(rw_lock_t *lock) {
  disable_interrupts();
  int old = atomic_load(lock);

  while (true) {
    if (old < 0)
      continue;

    if (atomic_cas(lock, old, old + 1)) {
      return;
    }
  }
}

void rw_read_release(rw_lock_t *lock) {
  atomic_sub(lock, 1);
  enable_interrupts();
}

void rw_write_acquire(rw_lock_t *lock) {
  disable_interrupts();
  while (!atomic_cas(lock, 0, -1))
    ;
}

void rw_write_release(rw_lock_t *lock) {
  atomic_store(lock, 0);
  enable_interrupts();
}
