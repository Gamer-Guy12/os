#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdatomic.h>
#include <stdint.h>
#include <stdbool.h>

typedef atomic_flag spinlock_t;

void spinlock_acquire(spinlock_t *lock);
void spinlock_release(spinlock_t *lock);

#define SPINLOCK_WITH(lock) for (spinlock_acquire(lock); false; spinlock_release(lock))

#endif
