#include "kernel/cores.h"
#include "kernel/threads.h"
#include "lib/list.h"
#include <stddef.h>
#include <stdint.h>

CLS(struct thread_queue, thread_queues);

struct thread *get_thread(struct thread_queue *queue) {
  struct list_node *node = queue->threads.next;
  if (node == NULL || node == &queue->threads) {
    return NULL;
  }

  list_remove(node);

  return (struct thread *)((uintptr_t)node - offsetof(struct thread, node));
}

void queue_thread(struct thread_queue *queue, struct thread *thread) {
  list_insert(queue->threads.prev, &thread->node);
}

struct thread *get_queued_thread(void) {
  struct thread_queue *queue = GET_CLS(thread_queues);

  return get_thread(queue);
}

void queue_cur_thread(struct thread *thread) {
  struct thread_queue *queue = GET_CLS(thread_queues);

  queue_thread(queue, thread);
}

