#include "interrupts.h"
#include "kernel/threads.h"
#include "lib/list.h"
#include "lib/spinlock.h"
#include <stdint.h>

void waitqueue_create(struct wait_queue *queue,
                      int (*check_thread)(struct thread *, void *)) {
  queue->check_thread = check_thread;
  LIST_INIT(&queue->waiting_threads);
  queue->wait_lock = (spinlock_t)SPINLOCK_ZERO(misc_wait);
}

void waitqueue_wait(struct wait_queue *queue) {
  temp_disable_interrupts();

  struct thread *thread = get_cur_thread();
  thread->wait_queue = queue;
  thread->state = THREAD_WAITING;

  schedule();
}

void waitqueue_awaken(struct wait_queue *queue, void *data) {
  struct list_node *pos = NULL;
  spinlock_acquire(&queue->wait_lock);
  LIST_FOREACH(pos, &queue->waiting_threads) {
    struct thread *thread =
        (struct thread *)((uintptr_t)pos - offsetof(struct thread, wait_node));

    int response = queue->check_thread(thread, data);
    bool accept = response & (1 << 1);
    bool cont = ~(response & 1);

    if (accept) {
      pos->next->prev = pos->prev;
      pos->prev->next = pos->next;

      schedule_thread(thread);
    }

    if (!cont)
      break;
  }
  spinlock_release(&queue->wait_lock);
}

bool waitqueue_destroy(struct wait_queue *queue, bool force) {
  spinlock_acquire(&queue->wait_lock);
  bool success = false;

  if (LIST_EMPTY(&queue->waiting_threads)) {
    success = true;
  } else if (force) {
    struct list_node *pos = NULL;
    LIST_FOREACH(pos, &queue->waiting_threads) {
      pos->next->prev = pos->prev;
      pos->prev->next = pos->next;

      struct thread *thread =
          (struct thread *)((uintptr_t)pos -
                            offsetof(struct thread, wait_node));

      schedule_thread(thread);
    }
    success = true;
  }
  spinlock_release(&queue->wait_lock);

  return success;
}

void __insert_wait_thread(struct thread *thread) {
  struct wait_queue *queue = thread->wait_queue;

  spinlock_acquire(&queue->wait_lock);
  list_insert(&queue->waiting_threads, &thread->wait_node);
  spinlock_release(&queue->wait_lock);
}
