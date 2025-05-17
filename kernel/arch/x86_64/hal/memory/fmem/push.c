#include <hal/imemory.h>
#include <libk/lock.h>
#include <stddef.h>
#include <stdint.h>

void *fmem_push(void) {
  lock_acquire(get_mem_lock());
  uint8_t *ptr = fmem_get_ptr(NULL);
  void *prev = ptr;
  ptr += PAGE_SIZE;
  fmem_get_ptr(ptr);
  lock_release(get_mem_lock());

  return prev;
}
