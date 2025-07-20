/// The queue node must be the first element in the struct so that it can be
/// recovered (unless u have another way)

#ifndef QUEUE_H
#define QUEUE_H

#include <libk/spinlock.h>

typedef struct queue_node_struct {
  struct queue_node_struct *next;
} queue_node_t;

typedef struct {
  queue_node_t *head;
  queue_node_t *tail;
} queue_t;

void queue_create(queue_t *queue);
void queue_enqueue(queue_t *queue, queue_node_t *element);
queue_node_t *queue_dequeue(queue_t *queue);

#endif
