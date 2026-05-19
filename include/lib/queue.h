#ifndef _LIB_QUEUE_H_
#define _LIB_QUEUE_H_

#include <stddef.h>

struct queue_node {
  struct queue_node *next;
};

struct queue {
  struct queue_node *head;
  struct queue_node *tail;
};

#define QUEUE_INIT(queue)                                                      \
  do {                                                                         \
    (queue)->head = NULL;                                                      \
    (queue)->tail = NULL;                                                      \
  } while (0);

#define QUEUE_CREATE(name) struct queue name = {.head = NULL, .tail = NULL}

void queue_enqueue(struct queue *queue, struct queue_node *node);
struct queue_node *queue_dequeue(struct queue *queue);

#endif
