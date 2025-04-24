#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <libk/lock.h>
#include <stdint.h>

typedef struct {
  uint32_t val;
  lock_t lock;
} semaphore_t;

void semaphore_wait(semaphore_t *semaphore);
void semaphore_signal(semaphore_t *semaphore);

#endif
