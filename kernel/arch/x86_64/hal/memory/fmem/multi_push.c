#include <hal/imemory.h>

void *fmem_multi_push(size_t count) {
  lock_acquire(get_mem_lock());
  uint8_t *ptr = fmem_get_ptr(NULL);
  void *prev = ptr;
  ptr += PAGE_SIZE * count;
  fmem_get_ptr(ptr);
  lock_release(get_mem_lock());

  return prev;
}
