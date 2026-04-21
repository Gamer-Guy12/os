#ifndef _LIB_SPINLOCK_H_
#define _LIB_SPINLOCK_H_

#include "lib/atomic.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
#ifdef _DEBUG_
  int current_core;
  char name[20];
#endif
  atomic_t val;
} spinlock_t;

void spinlock_acquire(spinlock_t *spinlock);
bool spinlock_attempt(spinlock_t *spinlock);
void spinlock_release(spinlock_t *spinlock);

#ifdef _DEBUG_
#define SPINLOCK(lock_name)                                                    \
  spinlock_t lock_name = {                                                     \
      .current_core = -1, .val = ATOMIC_ZERO, .name = #lock_name}

#define SPINLOCK_ZERO(lock_name)                                               \
  { .current_core = -1, .val = ATOMIC_ZERO, .name = #lock_name }
#else
#define SPINLOCK(lock_name) spinlock_t lock_name = {.val = ATOMIC_ZERO}

#define SPINLOCK_ZERO(lock_name)                                               \
  { .val = ATOMIC_ZERO }
#endif

#endif
