#include <libk/queue.h>
#include <stdbool.h>
#include <stddef.h>

queue_node_t *queue_dequeue(queue_t *queue) {
  if (queue->head == NULL) {
    return NULL;
  }

  while (true) {
    queue_node_t *head = queue->head;
    if (__sync_bool_compare_and_swap(&queue->head, head, head->next)) {
      return head;
    }
  }
}
