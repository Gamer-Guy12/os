#ifndef LOCK_H
#define LOCK_H

#include <stdint.h>

typedef volatile uint16_t spinlock_t;

void lock_acquire(spinlock_t *lock);
void lock_release(spinlock_t *lock);

/// This one will acquire and release the lock automatically
inline void lock_with(int (*func)(void), spinlock_t *lock) {
  lock_acquire(lock);
  func();
  lock_release(lock);
}

#endif
