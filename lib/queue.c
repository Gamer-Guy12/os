#include "lib/queue.h"
#include <stdbool.h>
#include <stddef.h>

void queue_enqueue(struct queue *queue, struct queue_node *node) {
  node->next = NULL;

  spinlock_acquire(&queue->lock);

  if (queue->head == NULL) {
    queue->head = node;
    queue->tail = node;
  } else {
    queue->tail->next = node;
    queue->tail = node;
  }

  spinlock_release(&queue->lock);
}

struct queue_node *queue_dequeue(struct queue *queue) {
  spinlock_acquire(&queue->lock);

  if (queue->head == queue->tail) {
    struct queue_node *node = queue->head;
    queue->head = NULL;
    queue->tail = NULL;
    spinlock_release(&queue->lock);
    return node;
  } else {
    struct queue_node *node = queue->head;
    queue->head = node->next;
    spinlock_release(&queue->lock);
    return node;
  }
}
