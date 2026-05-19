#ifndef _LIB_SPINLOCK_H_
#define _LIB_SPINLOCK_H_

#include "lib/atomic.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
#ifdef _DEBUG_
  int current_core;
  char name[20];
#endif
  // Used for ordering who is allowed to use the lock to maintain fairness
  // The next available index
  size_t cur_index;
  // The index that is currently running
  size_t running_index;
  // Only used to support spinlock_attempt (should probably move a way from it)
  atomic_t value;
} spinlock_t;

void _spinlock_acquire(spinlock_t *spinlock);
void _spinlock_release(spinlock_t *spinlock);

#ifdef _DEBUG_
#define SPINLOCK(lock_name)                                                    \
  spinlock_t lock_name = {.current_core = -1,                                  \
                          .name = #lock_name,                                  \
                          .cur_index = 0,                                      \
                          .running_index = 0,                                  \
                          .value = ATOMIC_ZERO}

#define SPINLOCK_ZERO(lock_name)                                               \
  {                                                                            \
    .current_core = -1, .name = #lock_name, .cur_index = 0,                    \
    .running_index = 0, .value = ATOMIC_ZERO                                   \
  }
#else
#define SPINLOCK(lock_name)                                                    \
  spinlock_t lock_name = {                                                     \
      .cur_index = 0, .running_index = 0, .value = ATOMIC_ZERO}

#define SPINLOCK_ZERO(lock_name)                                               \
  { .cur_index = 0, .running_index = 0, .value = ATOMIC_ZERO }
#endif

#endif
