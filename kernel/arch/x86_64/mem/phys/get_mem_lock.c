#include <libk/spinlock.h>
#include <mem/pimemory.h>
#include <stdatomic.h>

static spinlock_t lock = ATOMIC_FLAG_INIT;

spinlock_t *get_mem_lock(void) { return &lock; }
