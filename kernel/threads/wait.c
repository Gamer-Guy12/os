#include "kernel/gheap.h"
#include "kernel/mem.h"
#include "kernel/threads.h"
#include "lib/spinlock.h"
#include <stddef.h>

void waitqueue_create(struct wait_queue *queue) { LIST_INIT(&queue->list); }

void waitqueue_awaken(struct wait_queue *queue,
                      struct wait_queue_node *thread) {
  struct wait_queue_node *wait_node = NULL;

  spinlock_acquire(&queue->lock);

  struct list_node *node = NULL;
  LIST_FOREACH(node, &queue->list) {
    struct wait_queue_node *wnode = WQ_NODE(node);

    if (thread == wnode) {
      wait_node = wnode;
      list_remove(&wnode->node);
      break;
    }
  }

  thread->thread->state = THREAD_READY;
  schedule_thread(thread->thread);

  spinlock_release(&queue->lock);

  if (wait_node) gfree(wait_node);
}

void waitqueue_wait(struct wait_queue *queue) {
  struct wait_queue_node *node =
      gmalloc(sizeof(struct wait_queue_node), ZONE_ANY);
  struct thread *thread = get_cur_thread();
  thread->state = THREAD_WAITING;
  node->thread = thread;

  spinlock_acquire(&queue->lock);

  list_insert(&queue->list, &node->node);

  spinlock_release(&queue->lock);

  schedule();
}
