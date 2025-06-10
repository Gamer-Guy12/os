#include <libk/semaphore.h>
#include <libk/spinlock.h>
#include <stdint.h>

uint32_t semaphore_wait(semaphore_t *semaphore) {
  spinlock_acquire(&semaphore->lock);

  if (semaphore->val > 0) {
    semaphore->val--;

    uint32_t ret = semaphore->val;

    spinlock_release(&semaphore->lock);

    return ret;
  } else {
    spinlock_release(&semaphore->lock);

    while (semaphore->val < 1) {
      __asm__ volatile("pause" : : : "memory");
    }

    return semaphore_wait(semaphore);
  }
}
