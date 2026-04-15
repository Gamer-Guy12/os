#include "lib/queue.h"
#include "kernel/threads.h"
#include <stddef.h>
#include <stdint.h>

struct thread_queue global_queue;

void init_thread_queue(struct thread_queue *queue) {
  QUEUE_INIT(&queue->queue);
}

void init_global_thread_queue(void) { init_thread_queue(&global_queue); }

struct thread *pop_queue_thread(struct thread_queue *queue) {
  struct queue_node *node = queue_dequeue(&queue->queue);

  if (node == NULL)
    return NULL;

  uintptr_t thread = (uintptr_t)node - offsetof(struct thread, node);

  return (struct thread *)thread;
}

void queue_thread(struct thread_queue *queue, struct thread *thread) {
  queue_enqueue(&queue->queue, &thread->node);
}

void schedule_thread(struct thread *thread) {
  queue_enqueue(&global_queue.queue, &thread->node);
}

void requeue_thread(struct thread *thread) {
  schedule_thread(thread);
}

struct thread *pop_thread(void) {
  struct thread_queue *queue = &global_queue;
  struct queue_node *node = queue_dequeue(&queue->queue);

  if (node == NULL) {
    return NULL;
  }

  uintptr_t thread = (uintptr_t)node - offsetof(struct thread, node);

  return (struct thread *)thread;
}

void get_thread(void) {
}
