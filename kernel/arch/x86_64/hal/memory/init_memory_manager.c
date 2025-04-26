#include <hal/imemory.h>
#include <libk/lock.h>

void init_memory_manager(uint8_t *multiboot) {
  lock_acquire(get_mem_lock());

  lock_release(get_mem_lock());
}
