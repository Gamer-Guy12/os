#include "kernel/threads.h"
#include "lib/spinlock.h"
#include <stdbool.h>

void waitqueue_create(struct wait_queue *queue) {
  queue->lock = (spinlock_t)SPINLOCK_ZERO;
  LIST_INIT(&queue->list);
}

bool waitqueue_destroy(struct wait_queue *queue, bool force) {
  if (LIST_EMPTY(&queue->list)) {
    return true;
  } else if (force) {
    // If the list is empty and it is force
    struct list_node *node = NULL;

    LIST_FOREACH(node, &queue->list) {
      struct thread *thread = (struct thread *)((uintptr_t)node - offsetof(struct thread, wait_queue_node));

      waitqueue_awaken(queue, thread->tid);
    }

    return true;
  } else {
    return false;
  }
}

void waitqueue_awaken(struct wait_queue *queue, uint64_t tid) {
  struct thread* thread = thread_id(tid);
  struct list_node *node = NULL;
  bool found = false;

  LIST_FOREACH(node, &queue->list) {
    struct thread *list_thread = (struct thread *)((uintptr_t)node - offsetof(struct thread, wait_queue_node));
    if (thread->tid == list_thread->tid) {
      found = true;
      break;
    }
  }

  if (!found) return;

  list_remove(&thread->wait_queue_node);
  thread->wait_queue = NULL;
  thread->state = THREAD_READY;

  schedule_thread(thread);
}

void waitqueue_wait(struct wait_queue *queue) {
  struct thread* thread = get_cur_thread();

  thread->wait_queue = queue;
  thread->state = THREAD_WAITING;

  schedule();
}

