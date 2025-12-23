#include "lib/queue.h"
#include <stdbool.h>

void queue_enqueue(struct queue *queue, struct queue_node *node) {
  node->next = NULL;

  // Keep on going till you can add the tail to the end
  do {
    // Load the tail
    struct queue_node *tail = __atomic_load_n(&queue->tail, __ATOMIC_ACQUIRE);
    struct queue_node *next = tail->next;

    // If the tail isn't at the end
    if (tail->next != NULL) {
      // Advance the tail pionter to be pointing to the end
      __atomic_compare_exchange_n(&queue->tail, &tail, tail->next, false,
                                  __ATOMIC_RELEASE, __ATOMIC_ACQUIRE);
    } else {
      // Append to the end
      if (__atomic_compare_exchange_n(&tail->next, &next, node, false,
                                      __ATOMIC_RELEASE, __ATOMIC_ACQUIRE))
        break;
    }
  } while (1);

  struct queue_node *tail = __atomic_load_n(&queue->tail, __ATOMIC_ACQUIRE);
  // Advance the tail
  if (tail->next != NULL)
    __atomic_compare_exchange_n(&queue->tail, &tail, tail->next, false,
                                __ATOMIC_RELEASE, __ATOMIC_ACQUIRE);
}

struct queue_node *queue_dequeue(struct queue *queue) {
  do {
    struct queue_node *head = __atomic_load_n(&queue->head, __ATOMIC_ACQUIRE);

    // If the queue is empty return NULL
    if (head->next == NULL) {
      return NULL;
    }

    // Move head->next
    struct queue_node *next = head->next;

    if (__atomic_compare_exchange_n(&head->next, &next, next->next, false, __ATOMIC_RELEASE, __ATOMIC_ACQUIRE)) {
      return next;
    }
  } while (1);
}
