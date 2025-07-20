#include <libk/queue.h>
#include <stdbool.h>
#include <stddef.h>

void queue_enqueue(queue_t* queue, queue_node_t* element) {
  while (true) {
    queue_node_t* tail = queue->tail; 
    if (__sync_bool_compare_and_swap(&queue->tail, NULL, element)) {
      queue->head = element;
      break;
    }

    if (__sync_bool_compare_and_swap(&queue->tail, tail, element)) {
      tail->next = element; 
      break;
    }
  }
}

