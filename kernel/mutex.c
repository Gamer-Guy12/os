#include <threading.h>

void mutex_create(mutex_t *mutex) { semaphore_create(mutex, 1); }

void mutex_acquire(mutex_t *mutex) { semaphore_wait(mutex); }

void mutex_release(mutex_t *mutex) { semaphore_signal(mutex); }
