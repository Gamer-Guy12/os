#include <hal/imemory.h>
#include <libk/lock.h>

static lock_t lock;

lock_t *get_mem_lock(void) { return &lock; }
