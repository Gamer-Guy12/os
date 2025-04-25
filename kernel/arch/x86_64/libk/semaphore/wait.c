#include "libk/lock.h"
#include <libk/semaphore.h>

void semaphore_wait(semaphore_t *semaphore) {
  lock_acquire(&semaphore->lock);

  if (semaphore->val > 0) {
    semaphore->val--;

    lock_release(&semaphore->lock);
  } else {
    lock_release(&semaphore->lock);

    while (semaphore->val < 1) {
      __asm__ volatile("pause" : : : "memory");
    }

    semaphore_wait(semaphore);
  }
}
