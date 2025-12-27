#ifndef _LIB_QUEUE_H_
#define _LIB_QUEUE_H_

#include <stddef.h>

struct queue_node {
  struct queue_node *next;
};

struct queue {
  struct queue_node dummy;
  struct queue_node *head;
  struct queue_node *tail;
};

#define QUEUE_INIT(queue)                                                      \
  do {                                                                         \
    (queue)->dummy.next = NULL;                                                  \
    (queue)->head = &(queue)->dummy;                                               \
    (queue)->tail = &(queue)->dummy;                                               \
  } while (0);

#define QUEUE_CREATE(name)                                                     \
  struct queue name = {.dummy = {NULL}, .head = &name.dummy, .tail = &name.dummy}

void queue_enqueue(struct queue *queue, struct queue_node *node);
struct queue_node *queue_dequeue(struct queue *queue);

#endif
