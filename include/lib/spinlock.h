#ifndef _LIB_SPINLOCK_H_
#define _LIB_SPINLOCK_H_

#include "lib/atomic.h"
#include <stdbool.h>

typedef atomic_t spinlock_t;

void spinlock_acquire(spinlock_t *spinlock);
bool spinlock_attempt(spinlock_t *spinlock);
void spinlock_release(spinlock_t *spinlock);

#define SPINLOCK_ZERO ATOMIC_ZERO

#endif
