#include <libk/queue.h>
#include <libk/spinlock.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>
#include <threading/pcb.h>
#include <threading/tcb.h>
#include <threading/threading.h>

static queue_t thread_queue = {.head = NULL, .tail = NULL};

void queue_thread(TCB_t *tcb) { queue_enqueue(&thread_queue, &tcb->node); }

TCB_t *pop_thread(void) {
  const queue_node_t *node = queue_dequeue(&thread_queue);

  if (node == NULL) return NULL;

  const size_t offset = offsetof(TCB_t, node);
  
  return (void*)((uint8_t*)node - offset);
}
