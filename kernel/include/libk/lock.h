#ifndef LOCK_H
#define LOCK_H

#include <stdint.h>

typedef volatile uint16_t lock_t;

void lock_acquire(lock_t* lock);
void lock_release(lock_t* lock);

/// This one will acquire and release the lock automatically
inline void lock_with(int (*func)(void), lock_t* lock) {
    lock_acquire(lock);
    func();
    lock_release(lock);
}

#endif
