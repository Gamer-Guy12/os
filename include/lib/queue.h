#ifndef _LIB_QUEUE_H_
#define _LIB_QUEUE_H_

#include "spinlock.h"
#include <stddef.h>

struct queue_node {
  struct queue_node *next;
};

struct queue {
  struct queue_node *head;
  struct queue_node *tail;
  spinlock_t lock;
};

#define QUEUE_INIT(queue)                                                      \
  do {                                                                         \
    (queue)->head = NULL;                                                      \
    (queue)->tail = NULL;                                                      \
    (queue)->lock = (spinlock_t)SPINLOCK_ZERO(misc_queue);                     \
  } while (0);

#define QUEUE_CREATE(name)                                                     \
  struct queue name = {                                                        \
      .lock = (spinlock_t)SPINLOCK_ZERO(name), .head = NULL, .tail = NULL}

void queue_enqueue(struct queue *queue, struct queue_node *node);
struct queue_node *queue_dequeue(struct queue *queue);

#endif
