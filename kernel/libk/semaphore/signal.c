#include <libk/lock.h>
#include <libk/semaphore.h>

void semaphore_signal(semaphore_t *semaphore) {
  lock_acquire(&semaphore->lock);

  semaphore->val++;

  lock_release(&semaphore->lock);
}
