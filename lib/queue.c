#include "lib/queue.h"
#include <stdbool.h>
#include <stddef.h>

void queue_enqueue(struct queue *queue, struct queue_node *node) {
  node->next = NULL;

  if (queue->head == NULL) {
    queue->head = node;
    queue->tail = node;
  } else {
    queue->tail->next = node;
    queue->tail = node;
  }
}

struct queue_node *queue_dequeue(struct queue *queue) {
  if (queue->head == queue->tail) {
    struct queue_node *node = queue->head;
    queue->head = NULL;
    queue->tail = NULL;
    return node;
  } else {
    struct queue_node *node = queue->head;
    queue->head = node->next;
    return node;
  }
}
