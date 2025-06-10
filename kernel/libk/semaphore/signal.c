#include <libk/semaphore.h>
#include <libk/spinlock.h>

void semaphore_signal(semaphore_t *semaphore) {
  spinlock_acquire(&semaphore->lock);

  semaphore->val++;

  spinlock_release(&semaphore->lock);
}
