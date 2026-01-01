#include "kernel/threads.h"
#include "lib/spinlock.h"
#include <stddef.h>

void waitqueue_create(struct wait_queue *queue) {
  LIST_INIT(&queue->list);
}

void waitqueue_awaken(struct wait_queue *queue, struct wait_queue_node *thread) {
  spinlock_acquire(&queue->lock);

  struct list_node *node = NULL;
  LIST_FOREACH(node, &queue->list) {
    struct wait_queue_node *wnode = WQ_NODE(node);

    if (thread == wnode) {
      list_remove(&wnode->node);
      break;
    }
  }

  thread->thread->state = THREAD_READY;
  schedule_thread(thread->thread);

  spinlock_release(&queue->lock);
}

