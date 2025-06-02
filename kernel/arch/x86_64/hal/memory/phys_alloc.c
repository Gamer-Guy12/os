#include <hal/imemory.h>
#include <hal/memory.h>
#include <libk/kio.h>
#include <libk/lock.h>
#include <libk/math.h>
#include <stddef.h>

void *phys_alloc(void) {
  lock_acquire(get_mem_lock());

  lock_release(get_mem_lock());
  return NULL;
}
