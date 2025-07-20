#include <libk/kio.h>
#include <libk/queue.h>
#include <stdatomic.h>
#include <stddef.h>

void queue_create(queue_t *queue) {
  queue->head = NULL;
  queue->tail = NULL;
}
