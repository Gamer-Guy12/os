#include <apic_timer.h>
#include <asm.h>
#include <libk/queue.h>
#include <stddef.h>
#include <stdint.h>
#include <threading.h>
#include <threading/tcb.h>
#include <threading/threading.h>

void semaphore_create(semaphore_t *semaphore, int64_t max) {
  thread_queue_create(&semaphore->queue);

  semaphore->current_count = max;
}

void semaphore_wait(semaphore_t *semaphore) {
  /// This is the value before the subtraction
  int64_t held_value =
      __atomic_fetch_sub(&semaphore->current_count, 1, __ATOMIC_RELEASE);

  if (held_value > 0) {
    return;
  }

  TCB_t *tcb = TCB;

  tcb->state = THREAD_WAITING;
  queue_thread(tcb, tcb->priority, &semaphore->queue);
}

void semaphore_signal(semaphore_t *semaphore) {
  int64_t held_value =
      __atomic_fetch_add(&semaphore->current_count, 1, __ATOMIC_RELEASE);

  if (held_value < 0) {
    TCB_t *thread = pop_thread(&semaphore->queue);
    schedule_thread(thread, thread->priority == TP_IO ? TP_IO : TP_HIGH);
  }
}
