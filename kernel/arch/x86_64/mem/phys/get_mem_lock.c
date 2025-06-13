#include <libk/spinlock.h>
#include <mem/pimemory.h>

static spinlock_t lock;

spinlock_t *get_mem_lock(void) { return &lock; }
