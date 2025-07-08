#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdatomic.h>
#include <stdint.h>

typedef atomic_flag spinlock_t;

void spinlock_acquire(spinlock_t *lock);
void spinlock_release(spinlock_t *lock);

/// This one will acquire and release the lock automatically
inline void spinlock_with(int (*func)(void), spinlock_t *lock) {
  spinlock_acquire(lock);
  func();
  spinlock_release(lock);
}

#endif
