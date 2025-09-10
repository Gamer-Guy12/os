#include <asm.h>
#include <stdint.h>
#include <threading.h>
#include <threading/tcb.h>
#include <threading/threading.h>

void semaphore_create(semaphore_t *semaphore, int64_t max_value) {
  thread_queue_create(&semaphore->queue);

  semaphore->current_count = max_value;
}

/// Decrement Value
void semaphore_wait(semaphore_t *semaphore) {
  int64_t held_value = ATOMIC_DEC(semaphore->current_count);

  if (held_value > 0) {
    return;
  }

  TCB_t *thread = TCB;

  thread->state = THREAD_WAITING;

  queue_thread(thread, thread->priority, &semaphore->queue);
  run_next_thread();
}

/// Increment value
void semaphore_signal(semaphore_t *semaphore) {
  int64_t held_value = ATOMIC_INC(semaphore->current_count);

  if (held_value < 0) {
    TCB_t *thread = pop_thread(&semaphore->queue);

    thread_priority_t priority = thread->priority == TP_IO ? TP_IO : TP_HIGH;
    schedule_thread(thread, priority);
  }
}
