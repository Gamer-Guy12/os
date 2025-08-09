#include <asm.h>
#include <libk/queue.h>
#include <stdbool.h>
#include <stddef.h>

queue_node_t *queue_dequeue(queue_t *queue) {
  // while (true) {
  //   volatile queue_node_t *head = queue->head;
  //   volatile queue_node_t *tail = queue->tail;

  //   if (head == NULL) {
  //     return NULL;
  //   }

  //   if (head == tail) {
  //     if (tail->next != NULL) {
  //       __sync_bool_compare_and_swap(&queue->tail, tail, tail->next);
  //     } else {
  //       if (cmpxchg16b(&queue->head, (size_t)head, (size_t) NULL)) {
  //         return (queue_node_t *)head;
  //       }
  //     }
  //   }

  //   if (__sync_bool_compare_and_swap(&queue->head, head, head->next)) {
  //     return (queue_node_t *)head;
  //   }
  // }

  spinlock_acquire(&queue->lock);

  volatile queue_node_t *ret = queue->head;

  if (queue->head == queue->tail) {
    queue->head = NULL;
    queue->tail = NULL;
  }

  if (queue->head)
    queue->head = queue->head->next;

  spinlock_release(&queue->lock);

  __atomic_fetch_sub(&queue->count, 1, __ATOMIC_RELEASE);

  return (queue_node_t *)ret;
}
