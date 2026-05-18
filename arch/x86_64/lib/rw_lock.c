#include "lib/rw_lock.h"
#include "lib/atomic.h"
#include "util.h"
#include <stdbool.h>

void rw_read_acquire(rw_lock_t *lock) {
  RMEMB();
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
  WMEMB();
}

void rw_write_acquire(rw_lock_t *lock) {
  RMEMB();
  while (!atomic_cas(lock, 0, -1))
    ;
}

void rw_write_release(rw_lock_t *lock) {
  atomic_store(lock, 0);
  WMEMB();
}
