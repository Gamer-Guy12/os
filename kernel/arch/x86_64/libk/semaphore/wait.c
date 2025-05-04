#include <libk/lock.h>
#include <libk/semaphore.h>
#include <stdint.h>

uint32_t semaphore_wait(semaphore_t *semaphore) {
  lock_acquire(&semaphore->lock);

  if (semaphore->val > 0) {
    semaphore->val--;

    uint32_t ret = semaphore->val;

    lock_release(&semaphore->lock);

    return ret;
  } else {
    lock_release(&semaphore->lock);

    while (semaphore->val < 1) {
      __asm__ volatile("pause" : : : "memory");
    }

    return semaphore_wait(semaphore);
  }
}
