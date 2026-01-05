#ifndef _LIB_SPINLOCK_H_
#define _LIB_SPINLOCK_H_

#include "lib/atomic.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
#ifdef _DEBUG_
  int current_core;
#endif
  atomic_t val;
} spinlock_t;

void spinlock_acquire(spinlock_t *spinlock);
bool spinlock_attempt(spinlock_t *spinlock);
void spinlock_release(spinlock_t *spinlock);

#ifdef _DEBUG_
#define SPINLOCK_ZERO                                                          \
  { .current_core = -1, .val = ATOMIC_ZERO }
#else
#define SPINLOCK_ZERO                                                          \
  { .val = ATOMIC_ZERO }
#endif

#endif
