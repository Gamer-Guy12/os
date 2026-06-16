#include "kernel/threads.h"
#include "lib/list.h"
#include "lib/spinlock.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void waitqueue_create(struct waitqueue *queue,
                      int (*check_thread)(struct thread *, void *)) {
  queue->lock = (spinlock_t)SPINLOCK_ZERO(wait_queue_lock);
  queue->thread_count = 0;
  queue->check_thread = check_thread;
  LIST_INIT(&queue->threads);
}

void waitqueue_wait(struct waitqueue *queue) {
  struct thread *thread = get_cur_thread();
  thread->waitqueue = queue;
  thread->state = THREAD_WAITING;
  schedule();
}

void waitqueue_awaken(struct waitqueue *queue, void *param) {
  spinlock_acquire(&queue->lock);
  while (!LIST_EMPTY(&queue->threads)) {
    struct list_node *node = queue->threads.next;
    struct thread *thread =
        (struct thread *)((uintptr_t)node - offsetof(struct thread, wait_node));
    int result = queue->check_thread(thread, param);
    if (result & (1 << 0)) {
      queue->thread_count--;
      list_remove(&thread->wait_node);

      thread->state = THREAD_READY;
      schedule_thread(thread, thread->priority);
    }

    if (!(result & (1 << 1)))
      break;
  }
  spinlock_release(&queue->lock);
}

// Force will awaken all threads but if it is false then the destruction will
// fail
//
// 0 Failure
// 1 Success
int waitqueue_destroy(struct waitqueue *queue, bool force) {
  spinlock_acquire(&queue->lock);
  if (!force && queue->thread_count > 0) {
    spinlock_release(&queue->lock);
    return 0;
  }

  while (!LIST_EMPTY(&queue->threads)) {
    struct list_node *node = queue->threads.next;
    struct thread *thread =
        (struct thread *)((uintptr_t)node - offsetof(struct thread, wait_node));
    thread->state = THREAD_READY;
    schedule_thread(thread, thread->priority);
  }
  spinlock_release(&queue->lock);
  return 1;
}

void __do_wait(struct thread *thread) {
  struct waitqueue *queue = thread->waitqueue;
  spinlock_acquire(&queue->lock);
  queue->thread_count++;
  list_insert(&queue->threads, &thread->wait_node);
  spinlock_release(&queue->lock);
}
