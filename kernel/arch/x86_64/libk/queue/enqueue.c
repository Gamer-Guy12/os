#include <asm.h>
#include <libk/queue.h>
#include <stdbool.h>
#include <stddef.h>

void queue_enqueue(queue_t *queue, queue_node_t *element) {
  // while (true) {
  //   volatile queue_node_t *head = queue->head;
  //   volatile queue_node_t *tail = queue->tail;

  //   if (head == NULL && tail == NULL) {
  //     if (cmpxchg16b(&queue->head, (size_t)NULL, (size_t)element)) {
  //       break;
  //     }
  //     continue;
  //   }

  //   if (__sync_bool_compare_and_swap(&tail->next, NULL, element)) {
  //     // Move up the tail pointer
  //     __sync_bool_compare_and_swap(&queue->tail, tail, element);
  //     break;
  //   } else {
  //     __sync_bool_compare_and_swap(&queue->tail, tail, tail->next);
  //   }
  // }

  spinlock_acquire(&queue->lock);

  if (queue->head == queue->tail && queue->head == NULL) {
    queue->head = element;
    queue->tail = element;
  } else {
    if (queue->tail)
      queue->tail->next = element;
    queue->tail = element;
  }

  spinlock_release(&queue->lock);

  __atomic_fetch_add(&queue->count, 1, __ATOMIC_RELEASE);
}
