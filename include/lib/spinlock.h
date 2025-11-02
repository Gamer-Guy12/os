#ifndef _LIB_SPINLOCK_H_
#define _LIB_SPINLOCK_H_

#include "atomic.h"

typedef atomic_t spinlock_t;

void spinlock_acquire(spinlock_t *spinlock);
void spinlock_release(spinlock_t *spinlock);

#endif

