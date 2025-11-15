#ifndef _LIB_RW_LOCK_H_
#define _LIB_RW_LOCK_H_

#include "lib/atomic.h"

typedef atomic_t rw_lock_t;

void rw_read_acquire(rw_lock_t *lock);
void rw_read_release(rw_lock_t *lock);
void rw_write_acquire(rw_lock_t *lock);
void rw_write_release(rw_lock_t *lock);

#endif
