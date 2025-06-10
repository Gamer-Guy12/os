#ifndef LOCK_H
#define LOCK_H

#include <stdint.h>

typedef volatile uint16_t spinlock_t;

void spinlock_acquire(spinlock_t *lock);
void spinlock_release(spinlock_t *lock);

/// This one will acquire and release the lock automatically
inline void spinlock_with(int (*func)(void), spinlock_t *lock) {
  spinlock_acquire(lock);
  func();
  spinlock_release(lock);
}

#endif
