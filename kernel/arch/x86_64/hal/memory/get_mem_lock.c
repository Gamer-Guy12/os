#include <hal/pimemory.h>
#include <libk/lock.h>

static spinlock_t lock;

spinlock_t *get_mem_lock(void) { return &lock; }
