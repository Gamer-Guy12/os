#include "lib/queue.h"
#include "kernel/kprintf.h"
#include "kernel/threads.h"
#include "lib/spinlock.h"
#include <stddef.h>
#include <stdint.h>

static struct thread_queue global_queue;

void init_thread_queue(struct thread_queue *queue) {
  for (int i = 0; i < TP_COUNT; i++) {
    QUEUE_INIT(&queue->queues[i]);
  }
  queue->queue_lock = (spinlock_t)SPINLOCK_ZERO;
}

void init_thread_queues(void) { init_thread_queue(&global_queue); }

struct thread *pop_queue_thread(struct thread_queue *queue) {
  struct queue_node *node = NULL;

  spinlock_acquire(&queue->queue_lock);
  for (int i = 0; i < TP_COUNT; i++) {
    node = queue_dequeue(&queue->queues[i]);

    if (node != NULL)
      break;
  }
  spinlock_release(&queue->queue_lock);

  if (node == NULL) {
    return NULL;
  }

  return (struct thread *)((uintptr_t)node -
                           offsetof(struct thread, queue_node));
}

void enqueue_thread(struct thread_queue *queue, struct thread *thread) {
  spinlock_acquire(&queue->queue_lock);
  queue_enqueue(&queue->queues[thread->priority], &thread->queue_node);
  spinlock_release(&queue->queue_lock);
}

struct thread *pop_thread(void) { return pop_queue_thread(&global_queue); }

void schedule_thread(struct thread *thread) {
  enqueue_thread(&global_queue, thread);
}

void requeue_thread(struct thread *thread) {
  schedule_thread(thread);
}
