#include <hal/imemory.h>
#include <hal/memory.h>
#include <libk/lock.h>

void *phys_map(void) {
  lock_acquire(get_mem_lock());

  lock_release(get_mem_lock());
}
